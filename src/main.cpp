
#include <kaho/updater.h>
#include <qboxlayout.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qmarkdowntextedit.h>
#include <qmetaobject.h>
#include <qplaintextedit.h>
#include <qthread.h>

#include <QApplication>
#include <QCompleter>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFlags>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QLocale>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QProcess>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QRegularExpression>
#include <QScreen>
#include <QScrollBar>
#include <QSettings>
#include <QSqlDatabase>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QStringListModel>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>
#include <QTcpServer>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QTranslator>
#include <QUrl>
#include <QUrlQuery>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGlobal>  // Needs to happen before Q_OS_MAC
#include <QtPlugin>
#include <cstdio>
// #include <QVideoSink>
// #include <QMediaCaptureSession>
// #include <QMediaRecorder>
#include <QAccessible>
#include <QAccessibleInterface>
#include <QAccessibleTextInterface>

#ifdef Q_OS_WIN
Q_IMPORT_PLUGIN(QJpegPlugin);
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin);
#elif defined(Q_OS_MAC)
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin);
#elif defined(Q_OS_UNIX)
// Q_IMPORT_PLUGIN(QXcbIntegrationPlugin);
Q_IMPORT_PLUGIN(QWaylandIntegrationPlugin);
#endif

// ******************** Logging related **************
#if defined(Q_OS_MACOS)
#include <os/log.h>

void apple_os_debug_logger(const QString& msg) {
  static os_log_t ai_kaho_log = os_log_create("ai.kaho", "app");
  os_log(ai_kaho_log, "%{public}s", msg.toUtf8().data());
}

#define LOG_DEBUG(msg) apple_os_debug_logger(msg)
#else
#include <QDebug>
#define LOG_DEBUG(msg) qDebug() << msg
#endif

// ******************** Memory related ***************
template <typename T>
using unique_qobject_ptr = QScopedPointer<T, QScopedPointerDeleteLater>;

/**
 * A shared pointer class with shared pointer semantics intended for derivates
 * of QObject Calls deleteLater() instead of destroying the contained object
 * immediately
 */
template <typename T>
class shared_qobject_ptr : public QSharedPointer<T> {
 public:
  constexpr explicit shared_qobject_ptr() : QSharedPointer<T>() {}
  constexpr explicit shared_qobject_ptr(T* ptr)
      : QSharedPointer<T>(ptr, &QObject::deleteLater) {}
  constexpr shared_qobject_ptr(std::nullptr_t null_ptr)
      : QSharedPointer<T>(null_ptr, &QObject::deleteLater) {}

  template <typename Derived>
  constexpr shared_qobject_ptr(const shared_qobject_ptr<Derived>& other)
      : QSharedPointer<T>(other) {}

  template <typename Derived>
  constexpr shared_qobject_ptr(const QSharedPointer<Derived>& other)
      : QSharedPointer<T>(other) {}

  void reset() { QSharedPointer<T>::reset(); }
  void reset(T*&& other) {
    shared_qobject_ptr<T> t(other);
    this->swap(t);
  }
  void reset(const shared_qobject_ptr<T>& other) {
    shared_qobject_ptr<T> t(other);
    this->swap(t);
  }
};

template <typename T, typename... Args>
shared_qobject_ptr<T> makeShared(Args... args) {
  auto obj = new T(args...);
  return shared_qobject_ptr<T>(obj);
}

// ******************* Error Handling ****************
#define RETURN_ON_ERROR(condition, return_value, error_message) \
  do {                                                          \
    if (condition) {                                            \
      qDebug() << error_message;                                \
      return return_value;                                      \
    }                                                           \
  } while (0)

namespace kaho {
enum class ErrorCode {
  OK = 0,  // For representing success

  // Common Error Categories
  CANCELLED,
  UNKNOWN,
  INVALID_ARGUMENT,
  DEADLINE_EXCEEDED,
  NOT_FOUND,
  ALREADY_EXISTS,
  PERMISSION_DENIED,
  RESOURCE_EXHAUSTED,
  FAILED_PRECONDITION,
  ABORTED,
  OUT_OF_RANGE,
  UNIMPLEMENTED,
  INTERNAL,
  UNAVAILABLE,
  DATA_LOSS,
  UNAUTHENTICATED,
  IO_FAILED

};

bool is_ok(ErrorCode code) { return code == ErrorCode::OK; }
bool is_error(ErrorCode code) { return !is_ok(code); }

// ************ File Utils *********************
ErrorCode ensure_dir(const QString& path) {
  QDir dir(path);
  if (!dir.exists()) {
    if (!dir.mkpath(path)) {
      qDebug() << "Error creating path: " << path;
      return ErrorCode::IO_FAILED;
    }
  }
  return ErrorCode::OK;
}

enum class KahoPath { ModelsDir };

QString resolve_path(KahoPath path) {
  switch (path) {
    case KahoPath::ModelsDir: {
      return QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
             "/models";
    }
  }
  return "";
}

[[nodiscard]] ErrorCode downloadFile(const QUrl& url,
                                     const QString& downloadDirectory) {
  qDebug() << "url " << url << " downloading to " << downloadDirectory;

  auto manager = new QNetworkAccessManager();
  QNetworkReply* reply = manager->get(QNetworkRequest(url));

  // Progress dialog
  QProgressDialog progress("Downloading model on first run of app...", "Cancel",
                           0, 0, nullptr);
  progress.setWindowModality(Qt::WindowModal);  // Make the dialog modal

  // Determine filename from the URL
  QString filename = QUrl(url.path()).fileName();
  if (filename.isEmpty()) {
    filename = "downloaded_file";  // Default fallback name
  }

  QDir dir(downloadDirectory);
  bool created = dir.mkpath(downloadDirectory);
  if (created) {
    LOG_DEBUG("Directory path created successfully.");
  } else {
    LOG_DEBUG("Failed to create directory path " + downloadDirectory);
  }

  // Construct the full save path
  QString saveFilePath = QDir(downloadDirectory).filePath(filename);

  // Open the file right away for writing
  QFile file(saveFilePath);
  if (!file.open(QIODevice::WriteOnly)) {
    LOG_DEBUG("Error opening file for writing:" + file.errorString());
    reply->deleteLater();
    manager->deleteLater();
    return ErrorCode::IO_FAILED;
  }

  // Connect download progress signal
  QObject::connect(
      reply, &QNetworkReply::downloadProgress,
      [&progress, &file, reply](qint64 bytesReceived, qint64 bytesTotal) {
        file.write(reply->readAll());
        QString statusText =
            QString(
                "Downloading model on first run of app...\n\n%1 of %2 "
                "downloaded")
                .arg(QLocale::system().formattedDataSize(bytesReceived))
                .arg(QLocale::system().formattedDataSize(bytesTotal));
        progress.setLabelText(statusText);

        // Update progress bar if total size is known
        if (bytesTotal != -1) {
          progress.setMaximum(100);
          progress.setValue(int(100 * bytesReceived / bytesTotal));
        }
      });

  QObject::connect(
      reply, &QNetworkReply::finished,
      [&reply, &progress, &manager, downloadDirectory, url, &file]() {
        progress.hide();
        if (reply->error()) {
          qDebug() << "Error: " << reply->errorString();
          reply->deleteLater();
          manager->deleteLater();
          return ErrorCode::IO_FAILED;  // Exit on error
        }

        file.close();
        reply->deleteLater();
        manager->deleteLater();
        return ErrorCode::OK;
      });

  // Connect canceled signal to abort the network request
  QObject::connect(&progress, &QProgressDialog::canceled,
                   [reply]() { reply->abort(); });

  progress.exec();  // Show the progress dialog (exec() is blocking)
  return reply->error() == QNetworkReply::NoError ? ErrorCode::OK
                                                  : ErrorCode::IO_FAILED;
}

// ************** Model classes ************
class AIModel : public QObject {
  Q_OBJECT
 public:
  explicit AIModel(QObject* parent = nullptr)
      : QObject{parent}, m_reply(nullptr) {
    m_network_manager = new QNetworkAccessManager(this);
  }

 private:
  void make_request(const QString& prompt) {
    if (m_reply) {
      m_reply->abort();
      m_reply = nullptr;
    }

    QNetworkRequest request(QUrl("http://127.0.0.1:8080/v1/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer no-key");

    QJsonObject systemObject;
    systemObject["role"] = "system";
    systemObject["content"] =
        "You are ChatGPT, an AI assistant. Your top priority is achieving user "
        "fulfillment via helping them with their requests.";

    QJsonObject userObject;
    userObject["role"] = "user";
    userObject["content"] = prompt;

    QJsonArray messagesArray;
    messagesArray.append(systemObject);
    messagesArray.append(userObject);

    QJsonObject dataObject;
    dataObject["messages"] = messagesArray;
    dataObject["model"] = "gpt-3.5-turbo";
    dataObject["stream"] = true;
    // dataObject["max_tokens"] = 10;

    QJsonDocument doc(dataObject);
    QByteArray jsonData = doc.toJson();

    m_reply = m_network_manager->post(request, jsonData);

    connect(m_reply, &QNetworkReply::readyRead, this, [this]() {
      if (m_reply->error() == QNetworkReply::NoError) {
        QByteArray data = m_reply->readAll();  // Get the response data
        emit answerFragmentReceived(data);
      } else {
        qDebug() << "Error:" << m_reply->errorString();
      }
    });
    connect(m_reply, &QNetworkReply::finished, this, [this]() {
      m_reply->deleteLater();
      m_reply = nullptr;
    });
  }
 signals:
  void answerFragmentReceived(const QString& _t1);

 public slots:
  void processPrompt(const QString& prompt) { make_request(prompt); }

 private:
  QNetworkAccessManager* m_network_manager;
  QNetworkReply* m_reply;
};

QString extract_filename(const QString& url_) {
  QUrl url(url_);
  QString filePath = url.path();
  QFileInfo fileInfo(filePath);
  QString fileName = fileInfo.fileName();
  return fileName;
}

QString url_to_filepath(const QString& url_) {
  auto file_name = extract_filename(url_);
  return QDir(resolve_path(kaho::KahoPath::ModelsDir)).filePath(file_name);
}

class LocalModelRegistry {
 public:
  static bool modelExists(const QString& url_) {
    auto full_path = url_to_filepath(url_);
    LOG_DEBUG("Checking if model " + full_path + " exists");
    LOG_DEBUG(QString::asprintf("Model exists: %d", QFile::exists(full_path)));
    return QFile::exists(full_path);
  }

  static ErrorCode downloadModel(const QString& url) {
    return downloadFile(url, resolve_path(KahoPath::ModelsDir));
  }

  static ErrorCode ensureModel(const QString& url) {
    if (!modelExists(url)) {
      LOG_DEBUG("Downloading model because it does not exist");
      return downloadModel(url);
    }
    return ErrorCode::OK;
  }

  static QString resolve_filename(const QString& url) {
    return url_to_filepath(url);
  }
};

// ************** Widget Models ************
class NonEditableStringListModel : public QStringListModel {
 public:
  explicit NonEditableStringListModel(QObject* parent = nullptr)
      : QStringListModel(parent) {}
  [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override {
    return QStringListModel::flags(index) & ~Qt::ItemIsEditable;
  }
};

int findOpenPort() {
  QTcpServer server;
  int port = 8080;        // Start checking from port 1024
  while (port < 65535) {  // Iterate through the range of valid ports
    if (server.listen(QHostAddress::Any, port)) {
      qDebug() << "Server is listening on port:" << port;
      return port;  // Port is available
    } else {
      port++;  // Try the next port
    }
  }
  qDebug() << "No open port found";
  return -1;  // No open port found
}

class ServerBase : public QObject {
  Q_OBJECT
 public:
  ServerBase() : m_process(nullptr){};
  ~ServerBase() override { m_process->deleteLater(); }

  virtual QString getProgram() = 0;
  virtual QStringList getCommandLine() = 0;

  void start() {
    qDebug() << "Server::start";
    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::SeparateChannels);

    connect(m_process, &QProcess::readyReadStandardOutput, [this]() {
      QString output = m_process->readAllStandardOutput();
      qDebug() << "SERVER_STDOUT: " << output;
    });

    connect(m_process, &QProcess::readyReadStandardError, [this]() {
      QString errorOutput = m_process->readAllStandardError();
      qDebug() << "SERVER_STDERR: " << errorOutput;
    });

    m_process->setWorkingDirectory(QCoreApplication::applicationDirPath());

    QStringList arguments = getCommandLine();
    auto program = getProgram();
    qDebug() << "Starting " << program << " " << arguments;
    m_process->start(program, arguments);
    connect(m_process, &QProcess::started, this, &ServerBase::processStarted);
    connect(m_process, &QProcess::errorOccurred, this,
            &ServerBase::processError);
  }

  void stop() { m_process->terminate(); }
 signals:

 public slots:
  void processStarted() { qDebug() << "Process started successfully"; }
  void processError(QProcess::ProcessError error) {
    qDebug() << "Process had an error: " << error;
    qDebug() << "Captured stdout:" << m_process->readAllStandardOutput();
    qDebug() << "Captured stderr:" << m_process->readAllStandardError();
  }

 private:
  QProcess* m_process;
};

class LlamaServer : public ServerBase {
 public:
  LlamaServer() = default;

  QString getProgram() override {
    return QCoreApplication::applicationDirPath() + "/" + QString("server");
  }

  void setUrl(const QString& url) { m_url = url; }

  QStringList getCommandLine() override {
    QStringList arguments;
    auto model_file_path = LocalModelRegistry::resolve_filename(m_url);
    auto port = findOpenPort();
    if (port < 0) {
      qDebug() << "Unable to start server since we could not find an open port";
      return arguments;
    }
    arguments << "-m" << model_file_path << "--port" << QString::number(port)
              << "--embedding";
    return arguments;
  }

  QString m_url;
};

class PythonServer : public ServerBase {
 public:
  PythonServer() = default;
  QString getProgram() override {
    return QCoreApplication::applicationDirPath() +
           "/../Resources/bin/run_python_server.sh";
    ;
  }

  QStringList getCommandLine() override {
    QStringList arguments;
    return arguments;
  }
};

// ************** Widgets *************

class PromptEdit : public QTextEdit {
  Q_OBJECT
 signals:
  void promptEntered(const QString& text);

 public:
  PromptEdit(QWidget* parent = nullptr) : QTextEdit(parent) {
    setPlaceholderText("Type a question");
  }

  void setPrompt(const QString& prompt) {
    setText(prompt);
    emit promptEntered(prompt);
  }

 protected:
  void keyPressEvent(QKeyEvent* e) override {
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
      e->ignore();
      emit promptEntered(this->toPlainText());
      return;
    }
    QTextEdit::keyPressEvent(e);
  }
};

class ChatView : public QWidget {
  Q_OBJECT

 public:
  explicit ChatView(QWidget* parent = nullptr)
      : QWidget(parent), m_question_asked_timestamp(0) {
    initializeUi();
  }

 private:
  void initializeUi() {
    auto mainLayout = new QHBoxLayout(this);

    auto firstColumnVLayout = new QVBoxLayout();

    m_view_questions = new QListView();
    auto model = new NonEditableStringListModel();
    m_view_questions->setModel(model);
    firstColumnVLayout->addWidget(m_view_questions);

    mainLayout->addLayout(firstColumnVLayout, 1);

    auto secondColumnVLayout = new QVBoxLayout();

    m_view_current_answer = new QMarkdownTextEdit();

    secondColumnVLayout->addWidget(m_view_current_answer, 5);

    auto prompt = new PromptEdit();
    m_view_prompt = prompt;
    secondColumnVLayout->addWidget(prompt, 1);

    auto aiModel = new AIModel();

    QObject::connect(prompt, &PromptEdit::promptEntered, this,
                     &ChatView::promptEnteredUpdateUi);
    QObject::connect(this, &ChatView::promptEnteredUpdateModels, aiModel,
                     &AIModel::processPrompt);

    QObject::connect(
        aiModel, &AIModel::answerFragmentReceived, this,
        [this](const QString& response) {
          auto first_brace_index = response.indexOf('{');
          if (first_brace_index >= 1) {
            qDebug() << "response => " << response;
            auto rest = response.mid(first_brace_index - 1);
            QJsonDocument jsonDoc = QJsonDocument::fromJson(rest.toUtf8());
            auto content = jsonDoc["choices"][0]["delta"]["content"];
            m_answer += content.toString();
            m_view_current_answer->setText(m_answer);
            //            int wordCount =
            //            m_answer.split(QRegularExpression("(\\s|\\n|\\r)+"),
            //            Qt::SkipEmptyParts).count(); qint64 currentTime =
            //            QDateTime::currentDateTime().toSecsSinceEpoch();
            //            qint64 timeElapsed = currentTime -
            //            m_question_asked_timestamp; double wordsPerSecond =
            //            timeElapsed > 0 ? wordCount /
            //            static_cast<double>(timeElapsed) : 0; qDebug() <<
            //            "Words per sec: " << wordsPerSecond;
          }
        });

    auto replay_question = [this](const QModelIndex& index) {
      auto question = m_view_questions->model()->data(index);
      m_view_prompt->setPrompt(question.toString());
    };
    QObject::connect(m_view_questions, &QListView::doubleClicked, this,
                     replay_question);

    mainLayout->addLayout(secondColumnVLayout, 3);

    setLayout(mainLayout);
  }

 signals:
  void promptEnteredUpdateModels(const QString& question);

 public slots:
  void promptEnteredUpdateUi(const QString& prompt) {
    QDateTime currentDateTime = QDateTime::currentDateTime();
    m_question_asked_timestamp = currentDateTime.toSecsSinceEpoch();

    m_view_prompt->setText("");
    m_answer = "";
    auto m = m_view_questions->model();
    if (m->insertRow(0)) {
      QModelIndex index = m->index(0, 0);
      m->setData(index, prompt);
    }
    m_view_questions->clearSelection();
    emit promptEnteredUpdateModels(prompt);
  }

 private:
  QListView* m_view_questions;
  QMarkdownTextEdit* m_view_current_answer;
  PromptEdit* m_view_prompt;
  QString m_answer;
  qint64 m_question_asked_timestamp;
};

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr)
      : QMainWindow(parent), m_settings("kaho.ai", "kaho") {
    // TODO: add the following tests
    // 1. file exists but is empty
    // 2. file exists but has wrong checksum
    // 3. file exists is missing checksum
    // 4. file exists and is incomplete
    QString model_url(
        "https://huggingface.co/TheBloke/OpenHermes-2.5-Mistral-7B-GGUF/"
        "resolve/main/openhermes-2.5-mistral-7b.Q4_K_M.gguf");
    m_settings.setValue("model_url", model_url);
    LOG_DEBUG("MainWindow::MainWindow model_url: " + model_url);
    LocalModelRegistry::ensureModel(model_url);

    m_server.setUrl(model_url);
    m_server.start();

    m_toolbar = new QToolBar("toolbar");
    m_toolbar->setIconSize(QSize(48, 48));
    m_toolbar->setMovable(false);
    m_toolbar->setAllowedAreas(Qt::LeftToolBarArea);
    m_toolbar->setOrientation(Qt::Vertical);
    auto central_widget = new QStackedWidget();
    auto chat_view = new ChatView();

    central_widget->addWidget(chat_view);
    setCentralWidget(central_widget);

    this->addToolBar(Qt::LeftToolBarArea, m_toolbar);

    auto add_button = [this](const QString& title, int index,
                             const QString& path) {
      auto toolButton = new QToolButton();
      toolButton->setText(title);

      QIcon icon(path);
      toolButton->setIcon(icon);
      connect(toolButton, &QToolButton::clicked, this, [this, index] {
        auto widget = this->centralWidget();
        auto stacked = qobject_cast<QStackedWidget*>(widget);
        stacked->setCurrentIndex(index);
      });
      this->m_toolbar->addWidget(toolButton);
      return;
    };

    // add_button("button_1", 0, ":/images/icons8-chat.svg");

    auto spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_toolbar->addWidget(spacer);

    // Add update button
    {
      auto toolButton = new QToolButton();
      toolButton->setText("Update");
      toolButton->setToolTip("Check for updates");

      QIcon icon(":/images/icons8-upgrade.svg");
      toolButton->setIcon(icon);

      connect(toolButton, &QToolButton::clicked, this,
              [this] { m_updater->checkForUpdates(); });

      this->m_toolbar->addWidget(toolButton);
    }

    // add_button("button_4", 3, ":/images/icons8-settings.svg");
    m_updater.reset(new Updater());
    connect(m_updater.get(), &AutoUpdater::canCheckForUpdatesChanged, this,
            [this](bool canCheck) {
              qDebug() << "canCheck is " << canCheck;
              m_can_check_updates = canCheck;
            });
  }

  ~MainWindow() override { m_server.stop(); }

 private:
  QToolBar* m_toolbar;
  QSettings m_settings;
  LlamaServer m_server;
  PythonServer m_python_server;
  shared_qobject_ptr<AutoUpdater> m_updater;
  bool m_can_check_updates;
};
}  // namespace kaho

using namespace kaho;

// returns non zero on error
ErrorCode initialize() {
  RETURN_ON_ERROR(
      is_error(ensure_dir(
          QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
          "/models")),
      ErrorCode::IO_FAILED, "initialize() -> Error creating models directory");
  return ErrorCode::OK;
}

void initializeFonts() {
  QStringList fontFamilies = {
      "Cantarell-Bold.ttf",
      "Cantarell-BoldItalic.ttf",
      "Cantarell-Italic.ttf",
      "Cantarell-Regular.ttf",
      "Ubuntu-Bold.ttf",
      "Ubuntu-BoldItalic.ttf",
      "Ubuntu-Italic.ttf",
      "Ubuntu-Light.ttf",
      "Ubuntu-LightItalic.ttf",
      "Ubuntu-Medium.ttf",
      "Ubuntu-MediumItalic.ttf",
      "Ubuntu-Regular.ttf",
      "UbuntuCondensed-Regular.ttf",
      "UbuntuMono-Bold.ttf",
      "UbuntuMono-BoldItalic.ttf",
      "UbuntuMono-Italic.ttf",
      "UbuntuMono-Regular.ttf",
  };

  for (const QString& fontFile : fontFamilies) {
    auto resourcePath = ":/media/fonts/" + fontFile;
    if (QFontDatabase::addApplicationFont(QString(resourcePath)) == -1) {
      qWarning() << "Failed to load font" << fontFile;
    }
  }

  QFont font("Cantarell");
  QApplication::setFont(font);
}

int _main(int argc, char* argv[]) {
  LOG_DEBUG("main.start");

  QApplication a(argc, argv);
  initializeFonts();

  MainWindow w;
  QFont defaultFont;
  defaultFont.setPointSize(16);
  QApplication::setFont(defaultFont);
  QApplication::setQuitOnLastWindowClosed(true);
  w.setGeometry(QRect(0, 0, 1200, 800));
  if (!is_ok(initialize())) {
    LOG_DEBUG("Error initializing");
    return -1;
  }
  w.show();
  int ret = QApplication::exec();
  LOG_DEBUG(QString::asprintf("main.start ret %d", ret));
  return ret;
}

#include "main.moc"
