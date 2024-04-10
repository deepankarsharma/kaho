import os
import sys
from PySide6.QtWidgets import (QApplication, QMainWindow, QHBoxLayout, QVBoxLayout, QWidget,
                               QListView, QTextEdit, QStackedWidget, QToolBar, QToolButton)
from PySide6.QtCore import QSettings, Qt, QUrl, QFile, QIODevice, QDir, QObject, Signal
from PySide6.QtGui import QIcon, QFont, QFontDatabase
from PySide6.QtNetwork import QNetworkAccessManager, QNetworkRequest, QNetworkReply

class ErrorCode:
    OK = 0
    IO_FAILED = 1

def is_ok(code):
    return code == ErrorCode.OK

def is_error(code):
    return not is_ok(code)

def ensure_dir(path):
    dir = QDir(path)
    if not dir.exists():
        if not dir.mkpath(path):
            print(f"Error creating path: {path}")
            return ErrorCode.IO_FAILED
    return ErrorCode.OK

class KahoPath:
    ModelsDir = 1

def resolve_path(path):
    if path == KahoPath.ModelsDir:
        return QDir.toNativeSeparators(QDir.tempPath() + "/models")
    return ""

def download_file(url, download_directory):
    print(f"url {url} downloading to {download_directory}")
    manager = QNetworkAccessManager()
    reply = manager.get(QNetworkRequest(url))

    # Determine filename from the URL
    filename = QUrl(url.path()).fileName()
    if not filename:
        filename = "downloaded_file"  # Default fallback name

    dir = QDir(download_directory)
    created = dir.mkpath(download_directory)
    if created:
        print("Directory path created successfully.")
    else:
        print(f"Failed to create directory path {download_directory}")

    # Construct the full save path
    save_file_path = QDir(download_directory).filePath(filename)

    # Open the file right away for writing
    file = QFile(save_file_path)
    if not file.open(QIODevice.WriteOnly):
        print(f"Error opening file for writing: {file.errorString()}")
        reply.deleteLater()
        manager.deleteLater()
        return ErrorCode.IO_FAILED

    def on_download_progress(bytes_received, bytes_total):
        file.write(reply.readAll())
        print(f"Downloaded {bytes_received} of {bytes_total} bytes")

    reply.downloadProgress.connect(on_download_progress)

    def on_finished():
        if reply.error():
            print(f"Error: {reply.errorString()}")
            reply.deleteLater()
            manager.deleteLater()
            return ErrorCode.IO_FAILED
        file.close()
        reply.deleteLater()
        manager.deleteLater()
        return ErrorCode.OK

    reply.finished.connect(on_finished)
    return reply.error() == QNetworkReply.NoError

def extract_filename(url_):
    url = QUrl(url_)
    file_path = url.path()
    file_info = QFileInfo(file_path)
    file_name = file_info.fileName()
    return file_name

def url_to_filepath(url_):
    file_name = extract_filename(url_)
    return QDir(resolve_path(KahoPath.ModelsDir)).filePath(file_name)

class LocalModelRegistry:
    @staticmethod
    def model_exists(url_):
        full_path = url_to_filepath(url_)
        print(f"Checking if model {full_path} exists")
        print(f"Model exists: {QFile.exists(full_path)}")
        return QFile.exists(full_path)

    @staticmethod
    def download_model(url):
        return download_file(url, resolve_path(KahoPath.ModelsDir))

    @staticmethod
    def ensure_model(url):
        if not LocalModelRegistry.model_exists(url):
            print("Downloading model because it does not exist")
            return LocalModelRegistry.download_model(url)
        return ErrorCode.OK

    @staticmethod
    def resolve_filename(url):
        return url_to_filepath(url)

class PromptEdit(QTextEdit):
    promptEntered = Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setPlaceholderText("Type a question")

    def setPrompt(self, prompt):
        self.setText(prompt)
        self.promptEntered.emit(prompt)

    def keyPressEvent(self, e):
        if e.key() in (Qt.Key_Enter, Qt.Key_Return):
            e.ignore()
            self.promptEntered.emit(self.toPlainText())
            return
        super().keyPressEvent(e)

class ChatView(QWidget):
    promptEnteredUpdateModels = Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_answer = ""
        self.initializeUi()

    def initializeUi(self):
        mainLayout = QHBoxLayout(self)

        firstColumnVLayout = QVBoxLayout()
        self.m_view_questions = QListView()
        firstColumnVLayout.addWidget(self.m_view_questions)
        mainLayout.addLayout(firstColumnVLayout, 1)

        secondColumnVLayout = QVBoxLayout()
        self.m_view_current_answer = QTextEdit()
        secondColumnVLayout.addWidget(self.m_view_current_answer, 5)

        prompt = PromptEdit()
        self.m_view_prompt = prompt
        secondColumnVLayout.addWidget(prompt, 1)

        prompt.promptEntered.connect(self.promptEnteredUpdateUi)
        self.promptEnteredUpdateModels.connect(self.processPrompt)

        mainLayout.addLayout(secondColumnVLayout, 3)
        self.setLayout(mainLayout)

    def promptEnteredUpdateUi(self, prompt):
        self.m_view_prompt.setText("")
        self.m_answer = ""
        model = self.m_view_questions.model()
        if model.insertRow(0):
            index = model.index(0, 0)
            model.setData(index, prompt)
        self.m_view_questions.clearSelection()
        self.promptEnteredUpdateModels.emit(prompt)

    def processPrompt(self, question):
        # Placeholder for processing the prompt
        pass

class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.m_settings = QSettings("kaho.ai", "kaho")

        model_url = "https://example.com/model.bin"
        self.m_settings.setValue("model_url", model_url)
        print(f"MainWindow::MainWindow model_url: {model_url}")
        LocalModelRegistry.ensure_model(model_url)

        self.m_toolbar = QToolBar("toolbar")
        self.m_toolbar.setIconSize(48, 48)
        self.m_toolbar.setMovable(False)
        self.m_toolbar.setAllowedAreas(Qt.LeftToolBarArea)
        self.m_toolbar.setOrientation(Qt.Vertical)
        central_widget = QStackedWidget()
        chat_view = ChatView()
        central_widget.addWidget(chat_view)
        self.setCentralWidget(central_widget)

        self.addToolBar(Qt.LeftToolBarArea, self.m_toolbar)

        spacer = QWidget()
        spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.m_toolbar.addWidget(spacer)

    def add_button(self, title, index, path):
        toolButton = QToolButton()
        toolButton.setText(title)
        icon = QIcon(path)
        toolButton.setIcon(icon)
        toolButton.clicked.connect(lambda: self.centralWidget().setCurrentIndex(index))
        self.m_toolbar.addWidget(toolButton)

def initialize():
    if is_error(ensure_dir(QDir.toNativeSeparators(QDir.tempPath() + "/models"))):
        print("initialize() -> Error creating models directory")
        return ErrorCode.IO_FAILED
    return ErrorCode.OK

def initialize_fonts():
    font_families = [
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
    ]

    for font_file in font_families:
        resource_path = f":/media/fonts/{font_file}"
        if QFontDatabase.addApplicationFont(resource_path) == -1:
            print(f"Failed to load font {font_file}")

    font = QFont("Cantarell")
    QApplication.setFont(font)

if __name__ == "__main__":
    print("main.start")
    app = QApplication(sys.argv)
    initialize_fonts()

    main_window = MainWindow()
    default_font = QFont()
    default_font.setPointSize(16)
    QApplication.setFont(default_font)
    QApplication.setQuitOnLastWindowClosed(True)
    main_window.setGeometry(0, 0, 1200, 800)
    if not is_ok(initialize()):
        print("Error initializing")
        sys.exit(-1)
    main_window.show()
    ret = app.exec()
    print(f"main.start ret {ret}")
    sys.exit(ret)