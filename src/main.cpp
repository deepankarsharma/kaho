
#include <QtGlobal>  // Needs to happen before Q_OS_MAC
#include <kaho/updater.h>
#include <QApplication>
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
#include <QSettings>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QStringListModel>
#include <QStyle>
#include <QStyleFactory>
#include <QStyleHints>
#include <QTcpServer>
#include <QTextEdit>
#include <QThread>
#include <QToolButton>
#include <QToolBar>
#include <QTranslator>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include <qplaintextedit.h>
#include <qmetaobject.h>
#include <qthread.h>
#include <qboxlayout.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <cstdio>
#include <cassert>
#include <qmarkdowntextedit.h>
#include <QtPlugin>

#ifdef Q_OS_WIN
  Q_IMPORT_PLUGIN(QJpegPlugin);
  Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
  Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin);
#elif defined(Q_OS_MAC)
  //Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin);
#elif defined(Q_OS_UNIX)
  //Q_IMPORT_PLUGIN(QXcbIntegrationPlugin);
  Q_IMPORT_PLUGIN(QWaylandIntegrationPlugin);
#endif

// ******************** Memory related ***************
template <typename T>
using unique_qobject_ptr = QScopedPointer<T, QScopedPointerDeleteLater>;

/**
 * A shared pointer class with shared pointer semantics intended for derivates of QObject
 * Calls deleteLater() instead of destroying the contained object immediately
 */
template <typename T>
class shared_qobject_ptr : public QSharedPointer<T> {
 public:
  constexpr explicit shared_qobject_ptr() : QSharedPointer<T>() {}
  constexpr explicit shared_qobject_ptr(T* ptr) : QSharedPointer<T>(ptr, &QObject::deleteLater) {}
  constexpr shared_qobject_ptr(std::nullptr_t null_ptr) : QSharedPointer<T>(null_ptr, &QObject::deleteLater) {}

  template <typename Derived>
  constexpr shared_qobject_ptr(const shared_qobject_ptr<Derived>& other) : QSharedPointer<T>(other)
  {}

  template <typename Derived>
  constexpr shared_qobject_ptr(const QSharedPointer<Derived>& other) : QSharedPointer<T>(other)
  {}

  void reset() { QSharedPointer<T>::reset(); }
  void reset(T*&& other)
  {
    shared_qobject_ptr<T> t(other);
    this->swap(t);
  }
  void reset(const shared_qobject_ptr<T>& other)
  {
    shared_qobject_ptr<T> t(other);
    this->swap(t);
  }
};

template <typename T, typename... Args>
shared_qobject_ptr<T> makeShared(Args... args)
{
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

  // Construct the full save path
  QString saveFilePath = QDir(downloadDirectory).filePath(filename);

  // Open the file right away for writing
  QFile file(saveFilePath);
  if (!file.open(QIODevice::WriteOnly)) {
    qDebug() << "Error opening file for writing:" << file.errorString();
    reply->deleteLater();
    manager->deleteLater();
    return ErrorCode::IO_FAILED;
  }

  // Connect download progress signal
  QObject::connect(
      reply, &QNetworkReply::downloadProgress,
      [&progress, &file, reply](qint64 bytesReceived, qint64 bytesTotal) {
        file.write(reply->readAll());
        // Update label with download status
        // Update label with download status (human-readable)
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

  progress.exec();  // Show the progress dialog (exec() is blocking)
  return ErrorCode::OK;
}

// ************** Debug utilities **********



QWidget         *DEBUG_MESSAGE_DISPLAY_WIDGET   = NULL;
QPlainTextEdit  *DEBUG_MESSAGE_DISPLAY_TEXTEDIT = NULL;



void debugMessageDisplayFunc(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  QByteArray localMsg = msg.toLocal8Bit();
  const char *file = context.file ? context.file : "";
  const char *function = context.function ? context.function : "";
  switch (type) {
    case QtDebugMsg:
      fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
      break;
    case QtInfoMsg:
      fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
      break;
    case QtWarningMsg:
      fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
      break;
    case QtCriticalMsg:
      fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
      break;
    case QtFatalMsg:
      fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
      break;
  }
}

void setupDebugDisplay()
{
  auto widget = new QWidget();
  widget->setWindowTitle( "Debug Log" );
  widget->setAttribute( Qt::WA_QuitOnClose, false ); //quit only when mainwindow is closed
  QBoxLayout* layout = new QVBoxLayout();
  widget->setLayout( layout );
  auto textEdit = new QPlainTextEdit( widget );
  QFont font = QFont( "Monospace" );
  font.setStyleHint(QFont::TypeWriter);
  textEdit->setFont( font );
  textEdit->setReadOnly(true);
  layout->addWidget( textEdit );
  widget->show();
  DEBUG_MESSAGE_DISPLAY_WIDGET   = widget;
  DEBUG_MESSAGE_DISPLAY_TEXTEDIT = textEdit;
  qInstallMessageHandler(debugMessageDisplayFunc);
}



// ************** Markdown support *********
//QString markdown_to_html(const QString& markdown) {
//  cmark_gfm_core_extensions_ensure_registered();
//  cmark_parser *parser = cmark_parser_new(CMARK_OPT_DEFAULT);
//
//  const char* extensions[] = {"table", "autolink", "strikethrough"};
//
//  for (const char* extName : extensions) {
//    cmark_syntax_extension *ext = cmark_find_syntax_extension(extName);
//    if (ext) {
//      cmark_parser_attach_syntax_extension(parser, ext);
//    }
//  }
//
//  cmark_parser_feed(parser, markdown.toUtf8().constData(), markdown.length());
//  cmark_node *document = cmark_parser_finish(parser);
//  char *html = cmark_render_html(document, CMARK_OPT_DEFAULT, NULL);
//  QString html_qstring = QString::fromUtf8(html);
//  cmark_parser_free(parser);
//  cmark_node_free(document);
//  free(html);
//  return html_qstring;
//}

// ************** Model classes ************
class AIModel : public QObject {
  Q_OBJECT
 public:
  explicit AIModel(QObject* parent = nullptr) : QObject{parent}, m_reply(nullptr) {
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
    connect(m_reply, &QNetworkReply::finished, this,
            [this]() { m_reply->deleteLater(); m_reply = nullptr; });
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
    return QFile::exists(full_path);
  }
  static ErrorCode downloadModel(const QString& url) {
    return downloadFile(url, resolve_path(KahoPath::ModelsDir));
  }
  static ErrorCode ensureModel(const QString& url) {
    if (!modelExists(url)) {
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
  [[nodiscard]]
  Qt::ItemFlags flags(const QModelIndex& index) const override {
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

class Server : public QObject {
  Q_OBJECT
 public:
  Server() : m_process(nullptr) {};
  ~Server() override {
      m_process->deleteLater();
  }

  void start(const QString& url) {
    qDebug() << "Server::start";
    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::SeparateChannels);

    // Connect signals to slots (functions)
    connect(m_process, &QProcess::readyReadStandardOutput, [this]() {
      QString output = m_process->readAllStandardOutput();
      qDebug() << "SERVER_STDOUT: " << output;
    });

    connect(m_process, &QProcess::readyReadStandardError, [this]() {
      QString errorOutput = m_process->readAllStandardError();
      qDebug() << "SERVER_STDERR: " << errorOutput;
    });

    m_process->setWorkingDirectory(QCoreApplication::applicationDirPath());
    QStringList arguments;
    auto model_file_path = LocalModelRegistry::resolve_filename(url);
    auto port = findOpenPort();
    if (port < 0) {
      qDebug() << "Unable to start server since we could not find an open port";
      return;
    }
    arguments << "-m" << model_file_path << "--port" << QString::number(port);
    auto program =
        QCoreApplication::applicationDirPath() + "/" + QString("server");
    qDebug() << "Starting " << program << " " << arguments;
    m_process->start(program, arguments);
    connect(m_process, &QProcess::started, this, &Server::processStarted);
    connect(m_process, &QProcess::errorOccurred, this, &Server::processError);
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

// ************** Widgets *************

class PromptEdit : public QTextEdit {
  Q_OBJECT
 public:
  PromptEdit() { this->setPlaceholderText("Type a question"); }
  void setPrompt(const QString& prompt) {
    setText(prompt);
    emit promptEntered(prompt);
  }
 protected:
  void keyPressEvent(QKeyEvent* event) override {
    if (event->key() == Qt::Key_Return &&
        event->modifiers() == Qt::ShiftModifier) {
      qDebug() << "PromptEdit::keyPressEvent => TODO: handle <Shift-Enter>";
    } else if (event->key() == Qt::Key_Return) {
      qDebug() << "PromptEdit::keyPressEvent => " << this->toPlainText();
      emit promptEntered(this->toPlainText());
    } else {
      QTextEdit::keyPressEvent(event);
    }
  }

 signals:
  void promptEntered(const QString& text);
};

class ChatView : public QWidget {
  Q_OBJECT

 public:
  explicit ChatView(QWidget* parent = nullptr) : QWidget(parent) { initializeUi(); }

 private:
  void initializeUi() {
    // clang-format off
            // ┌───────────────────────────────────────────────────────────────────────────────────────────┐
            // │                                                                                           │
            // │  ┌─────────────────────────────────────────────┐  ┌─────────────────────────────────────┐ │
            // │  │  firstColumnLayout                          │  │  secondColumnLayout                 │ │
            // │  │                                             │  │ ┌────────────────────┐              │ │
            // │  │ ┌─────────────────────────────────────────┐ │  │ │questionLabel       │              │ │
            // │  │ │ m_view_questions                        │ │  │ │                    │              │ │
            // │  │ │                                         │ │  │ └────────────────────┘              │ │
            // │  │ │                                         │ │  │                                     │ │
            // │  │ │                                         │ │  │ ┌─────────────────────────────────┐ │ │
            // │  │ │                                         │ │  │ │m_view_current_question          │ │ │
            // │  │ │                                         │ │  │ │                                 │ │ │
            // │  │ │                                         │ │  │ │                                 │ │ │
            // │  │ │                                         │ │  │ │                                 │ │ │
            // │  │ │                                         │ │  │ │                                 │ │ │
            // │  │ │                                         │ │  │ │                                 │ │ │
            // │  │ │                                         │ │  │ └─────────────────────────────────┘ │ │
            // │  │ │                                         │ │  │                                     │ │
            // │  │ │                                         │ │  │ ┌─────────────────────┐             │ │
            // │  │ │                                         │ │  │ │answerLabel          │             │ │
            // │  │ │                                         │ │  │ │                     │             │ │
            // │  │ │                                         │ │  │ └─────────────────────┘             │ │
            // │  │ │                                         │ │  │                                     │ │
            // │  │ │                                         │ │  │ ┌─────────────────────────────────┐ │ │
            // │  │ │                                         │ │  │ │m_view_current_answer            │ │ │
            // │  │ │                                         │ │  │ │                                 │ │ │
            // │  │ │                                         │ │  │ │                                 │ │ │
            // │  │ │                                         │ │  │ └─────────────────────────────────┘ │ │
            // │  │ │                                         │ │  │                                     │ │
            // │  │ │                                         │ │  │ ┌─────────────────────────────────┐ │ │
            // │  │ │                                         │ │  │ │m_view_prompt                    │ │ │
            // │  │ │                                         │ │  │ │                                 │ │ │
            // │  │ └─────────────────────────────────────────┘ │  │ └─────────────────────────────────┘ │ │
            // │  │                                             │  │                                     │ │
            // │  └─────────────────────────────────────────────┘  └─────────────────────────────────────┘ │
            // │                                                                                           │
            // └───────────────────────────────────────────────────────────────────────────────────────────┘
    // clang-format on
    auto mainLayout = new QHBoxLayout(this);

    auto firstColumnVLayout = new QVBoxLayout();

    m_view_questions = new QListView();
    auto model = new NonEditableStringListModel();
    m_view_questions->setModel(model);
    firstColumnVLayout->addWidget(m_view_questions);

    mainLayout->addLayout(firstColumnVLayout, 1);

    auto secondColumnVLayout = new QVBoxLayout();

    m_view_current_answer = new QMarkdownTextEdit();
//    auto font = m_view_current_answer->font();
//    font.setPointSize(font.pointSize() * 4);
    //m_view_current_answer->setFont(font);

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
        aiModel, &AIModel::answerFragmentReceived,
        [this](const QString& response) {
          auto first_brace_index = response.indexOf('{');
          if (first_brace_index >= 1) {
            auto rest = response.mid(first_brace_index - 1);
            QJsonDocument jsonDoc = QJsonDocument::fromJson(rest.toUtf8());
            auto content = jsonDoc["choices"][0]["delta"]["content"];
            m_answer += content.toString();
            //qDebug() << "answer ==> " << this->m_answer;
            m_view_current_answer->setText(m_answer);
            //auto html = markdown_to_html(m_answer);
            //qDebug() << "rendered_html ==> " << html;
            //this->m_view_current_answer->setText(m_answer);
            //this->m_view_current_answer->setHtml(html);
          }
        });

    auto replay_question = [this](const QModelIndex& index) {
      auto question = m_view_questions->model()->data(index);
      m_view_prompt->setPrompt(question.toString());
    };
    QObject::connect(
        m_view_questions,
        &QListView::doubleClicked,
        this,
        replay_question);

    mainLayout->addLayout(secondColumnVLayout, 3);

    setLayout(mainLayout);
  }

 signals:
  void promptEnteredUpdateModels(const QString& question);

 public slots:
  void promptEnteredUpdateUi(const QString& prompt) {
    m_view_prompt->setText("");
    //m_view_current_answer->setText("");
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

    LocalModelRegistry::ensureModel(model_url);
    m_server.start(model_url);

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
      connect(toolButton, &QToolButton::clicked, [this, index] {
        auto widget = this->centralWidget();
        auto stacked = qobject_cast<QStackedWidget*>(widget);
        stacked->setCurrentIndex(index);
      });
      this->m_toolbar->addWidget(toolButton);
      return;
    };

    //add_button("button_1", 0, ":/images/icons8-chat.svg");

    auto spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_toolbar->addWidget(spacer);

    // Add update button
    {
      auto toolButton = new QToolButton();
      toolButton->setText("Update");

      QIcon icon(":/images/icons8-upgrade.svg");
      toolButton->setIcon(icon);

      connect(toolButton, &QToolButton::clicked, [this] {
        m_updater->checkForUpdates();
      });

      this->m_toolbar->addWidget(toolButton);
    }

    //add_button("button_4", 3, ":/images/icons8-settings.svg");
    m_updater.reset(new Updater());
    connect(m_updater.get(), &AutoUpdater::canCheckForUpdatesChanged, this, [this](bool canCheck) {
      qDebug() << "canCheck is " << canCheck;
      m_can_check_updates = canCheck;
    });
  }

  ~MainWindow() override { m_server.stop(); }

 private:
  QToolBar* m_toolbar;
  QSettings m_settings;
  Server m_server;
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
    // Prepend ":/" to indicate the font is in a resource file
    auto resourcePath = ":/media/fonts/" + fontFile;
    qDebug() << "Loading font " << resourcePath;
    if (QFontDatabase::addApplicationFont(QString(resourcePath)) == -1) {
      qWarning() << "Failed to load font" << fontFile;
    }
  }

  QFont font("Cantarell-Regular");
  QApplication::setFont(font);
}

int _main(int argc, char* argv[]) {
  qDebug() << "Main";

  QApplication a(argc, argv);
  initializeFonts();

  MainWindow w;
  QFont defaultFont;
  defaultFont.setPointSize(16);
  QApplication::setFont(defaultFont);
  QApplication::setQuitOnLastWindowClosed( true );

  // setupDebugDisplay();

  w.setGeometry(QRect(0, 0, 1200, 800));
  if (!is_ok(initialize())) {
    qDebug() << "Error initializing";
    return -1;
  }
  w.show();

  int ret = QApplication::exec();
  delete DEBUG_MESSAGE_DISPLAY_WIDGET;
  return ret;
}

#include "main.moc"