#include <QApplication>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QThread>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QPainter>
#include <QPropertyAnimation>
#include <QEasingCurve>

#include "pkg_tool/lib.h"

// Custom Toggle Switch Widget
class ToggleSwitch : public QWidget {
    Q_OBJECT

public:
    ToggleSwitch(QWidget *parent = nullptr) : QWidget(parent), m_checked(true), m_position(1.0) {
        setFixedSize(60, 30);
        setCursor(Qt::PointingHandCursor);
        
        m_animation = new QPropertyAnimation(this, "position", this);
        m_animation->setDuration(200);
        m_animation->setEasingCurve(QEasingCurve::InOutCubic);
    }
    
    bool isChecked() const { return m_checked; }
    
    void setChecked(bool checked) {
        if (m_checked != checked) {
            m_checked = checked;
            m_animation->setStartValue(m_position);
            m_animation->setEndValue(m_checked ? 1.0 : 0.0);
            m_animation->start();
            emit toggled(m_checked);
        }
    }
    
    Q_PROPERTY(qreal position READ position WRITE setPosition)
    
    qreal position() const { return m_position; }
    void setPosition(qreal pos) { m_position = pos; update(); }

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        
        // Background track
        QRect trackRect = rect().adjusted(2, 2, -2, -2);
        QColor trackColor = m_checked ? QColor(76, 175, 80) : QColor(100, 100, 100);
        p.setBrush(trackColor);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(trackRect, 13, 13);
        
        // Thumb
        int thumbSize = 22;
        int thumbY = (height() - thumbSize) / 2;
        int thumbX = 4 + (int)((width() - thumbSize - 8) * m_position);
        
        QRect thumbRect(thumbX, thumbY, thumbSize, thumbSize);
        p.setBrush(QColor(255, 255, 255));
        p.setPen(QPen(QColor(200, 200, 200), 1));
        p.drawEllipse(thumbRect);
    }
    
    void mousePressEvent(QMouseEvent *) override {
        setChecked(!m_checked);
    }

private:
    bool m_checked;
    qreal m_position;
    QPropertyAnimation *m_animation;
};

// Worker class for extraction
class ExtractWorker : public QObject {
    Q_OBJECT

public:
    ExtractWorker(const QString &pkgPath, const QString &outputPath) 
        : pkgPath(pkgPath), outputPath(outputPath) {}

signals:
    void progress(uint32_t extracted, uint32_t total);
    void finished(QString error);

public slots:
    void run() {
        auto result = ExtractPkg(
            pkgPath.toStdString(), 
            outputPath.toStdString(),
            {}, // Extract all files
            [this](uint32_t e, uint32_t t) { emit progress(e, t); }
        );
        
        emit finished(result ? QString::fromStdString(*result) : "");
    }

private:
    QString pkgPath;
    QString outputPath;
};

// Main Widget
class MainWidget : public QWidget {
    Q_OBJECT

public:
    MainWidget() {
        setWindowTitle("PS4 PKG Tool");
        setMinimumSize(500, 400);
        resize(600, 500);
        setAcceptDrops(true);
        setupUI();
        applyTheme();
    }

protected:
    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasUrls()) {
            event->acceptProposedAction();
        }
    }
    
    void dropEvent(QDropEvent *event) override {
        const QMimeData *mimeData = event->mimeData();
        if (mimeData->hasUrls()) {
            QList<QUrl> urls = mimeData->urls();
            if (!urls.isEmpty()) {
                QString filePath = urls.first().toLocalFile();
                if (filePath.endsWith(".pkg", Qt::CaseInsensitive)) {
                    startSingleExtraction(filePath);
                }
            }
        }
    }

private slots:
    void selectSinglePkg() {
        QString file = QFileDialog::getOpenFileName(this, "Select PKG File", "", "PKG Files (*.pkg)");
        if (!file.isEmpty()) {
            startSingleExtraction(file);
        }
    }
    
    void selectBatchDir() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Directory with PKG Files");
        if (!dir.isEmpty()) {
            startBatchExtraction(dir);
        }
    }

private:
    void setupUI() {
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(30);
        mainLayout->setContentsMargins(40, 40, 40, 40);
        
        // Title
        auto *titleLabel = new QLabel("PS4 PKG Extraction Tool");
        titleLabel->setObjectName("titleLabel");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);
        
        // Main instruction
        auto *instructionLabel = new QLabel("Drag & drop a PKG file here to start extraction\nor use the buttons below");
        instructionLabel->setObjectName("instructionLabel");
        instructionLabel->setAlignment(Qt::AlignCenter);
        instructionLabel->setWordWrap(true);
        mainLayout->addWidget(instructionLabel);
        
        mainLayout->addStretch();
        
        // Buttons
        auto *buttonLayout = new QVBoxLayout;
        buttonLayout->setSpacing(15);
        
        auto *singleBtn = new QPushButton("📁 Select Single PKG File");
        singleBtn->setObjectName("primaryButton");
        singleBtn->setMinimumHeight(50);
        connect(singleBtn, &QPushButton::clicked, this, &MainWidget::selectSinglePkg);
        buttonLayout->addWidget(singleBtn);
        
        auto *batchBtn = new QPushButton("📂 Batch Extract from Directory");
        batchBtn->setObjectName("secondaryButton");
        batchBtn->setMinimumHeight(50);
        connect(batchBtn, &QPushButton::clicked, this, &MainWidget::selectBatchDir);
        buttonLayout->addWidget(batchBtn);
        
        mainLayout->addLayout(buttonLayout);
        mainLayout->addStretch();
        
        // Output Directory Setting (moved to bottom)
        auto *settingLayout = new QHBoxLayout;
        settingLayout->setSpacing(15);
        settingLayout->setAlignment(Qt::AlignCenter);
        
        auto *settingLabel = new QLabel("Output Directory:");
        settingLabel->setObjectName("settingLabel");
        settingLayout->addWidget(settingLabel);
        
        outputToggle = new ToggleSwitch();
        outputToggle->setChecked(true);
        settingLayout->addWidget(outputToggle);
        
        auto *toggleLabel = new QLabel("Extract to same folder as PKG file");
        toggleLabel->setObjectName("toggleLabel");
        settingLayout->addWidget(toggleLabel);
        
        settingLayout->addStretch();
        mainLayout->addLayout(settingLayout);
        
        // Helper text for the setting
        auto *helperLabel = new QLabel("When unchecked, you'll be asked to choose an output directory");
        helperLabel->setObjectName("helperLabel");
        helperLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(helperLabel);
        
        // Progress bar (initially hidden)
        progressBar = new QProgressBar;
        progressBar->setObjectName("progressBar");
        progressBar->setVisible(false);
        mainLayout->addWidget(progressBar);
    }
    
    void applyTheme() {
        setStyleSheet(R"(
            QWidget {
                background-color: #2b2b2b;
                color: #ffffff;
                font-family: 'Segoe UI', 'Ubuntu', sans-serif;
                font-size: 12px;
            }
            
            #titleLabel {
                font-size: 28px;
                font-weight: bold;
                color: #4CAF50;
                margin: 20px 0px;
            }
            
            #instructionLabel {
                font-size: 16px;
                color: #cccccc;
                margin: 20px 0px;
                line-height: 1.4;
            }
            
            #settingLabel {
                font-size: 14px;
                font-weight: bold;
                color: #ffffff;
            }
            
            #toggleLabel {
                font-size: 14px;
                color: #ffffff;
            }
            
            #helperLabel {
                font-size: 12px;
                color: #888888;
                font-style: italic;
                margin-bottom: 10px;
            }
            
            #primaryButton {
                background-color: #4CAF50;
                border: 2px solid #388E3C;
                border-radius: 8px;
                color: white;
                font-size: 14px;
                font-weight: bold;
                padding: 15px;
            }
            
            #primaryButton:hover {
                background-color: #45a049;
                border-color: #2E7D32;
            }
            
            #primaryButton:pressed {
                background-color: #2E7D32;
            }
            
            #secondaryButton {
                background-color: #2196F3;
                border: 2px solid #1976D2;
                border-radius: 8px;
                color: white;
                font-size: 14px;
                font-weight: bold;
                padding: 15px;
            }
            
            #secondaryButton:hover {
                background-color: #1E88E5;
                border-color: #1565C0;
            }
            
            #secondaryButton:pressed {
                background-color: #1565C0;
            }
            
            #progressBar {
                background-color: #404040;
                border: 1px solid #555555;
                border-radius: 4px;
                text-align: center;
                color: white;
            }
            
            #progressBar::chunk {
                background-color: #4CAF50;
                border-radius: 3px;
            }
        )");
    }
    
    void startSingleExtraction(const QString &pkgPath) {
        QString outputPath;
        if (outputToggle->isChecked()) {
            QFileInfo info(pkgPath);
            QString baseName = info.completeBaseName(); // Gets filename without .pkg extension
            outputPath = info.dir().absoluteFilePath(baseName); // parent_dir/CUSA47498
        } else {
            QString baseDir = QFileDialog::getExistingDirectory(this, "Select Output Directory");
            if (baseDir.isEmpty()) return;
            QFileInfo info(pkgPath);
            QString baseName = info.completeBaseName();
            outputPath = QDir(baseDir).absoluteFilePath(baseName);
        }
        
        extractPkg(pkgPath, outputPath);
    }
    
    void startBatchExtraction(const QString &dirPath) {
        QDir dir(dirPath);
        QStringList pkgFiles = dir.entryList(QStringList() << "*.pkg", QDir::Files);
        
        if (pkgFiles.isEmpty()) {
            QMessageBox::warning(this, "No PKG Files", "No PKG files found in the selected directory.");
            return;
        }
        
        QString baseOutputPath;
        if (!outputToggle->isChecked()) {
            baseOutputPath = QFileDialog::getExistingDirectory(this, "Select Output Directory");
            if (baseOutputPath.isEmpty()) return;
        }
        
        // Extract each PKG file
        for (const QString &pkgFile : pkgFiles) {
            QString fullPath = dir.absoluteFilePath(pkgFile);
            QFileInfo info(fullPath);
            QString baseName = info.completeBaseName(); // PKG name without extension
            
            QString pkgOutputPath;
            if (outputToggle->isChecked()) {
                pkgOutputPath = dir.absoluteFilePath(baseName); // same dir + PKG name
            } else {
                pkgOutputPath = QDir(baseOutputPath).absoluteFilePath(baseName); // chosen dir + PKG name
            }
            
            extractPkg(fullPath, pkgOutputPath);
        }
    }
    
    void extractPkg(const QString &pkgPath, const QString &outputPath) {
        progressBar->setVisible(true);
        progressBar->setValue(0);
        progressBar->setFormat("Extracting...");
        
        auto *thread = new QThread;
        auto *worker = new ExtractWorker(pkgPath, outputPath);
        worker->moveToThread(thread);
        
        connect(thread, &QThread::started, worker, &ExtractWorker::run);
        connect(worker, &ExtractWorker::progress, this, [this](uint32_t e, uint32_t t){
            progressBar->setMaximum(static_cast<int>(t));
            progressBar->setValue(static_cast<int>(e));
        });
        connect(worker, &ExtractWorker::finished, this, [this, thread, worker](QString err){
            if (err.isEmpty()) {
                progressBar->setFormat("✅ Extraction Complete!");
                QTimer::singleShot(2000, [this]() { progressBar->setVisible(false); });
            } else {
                progressBar->setVisible(false);
                QMessageBox::critical(this, "Extraction Error", "Failed to extract PKG:\n" + err);
            }
            thread->quit();
            thread->wait();
            thread->deleteLater();
            worker->deleteLater();
        });
        
        thread->start();
    }

private:
    QProgressBar *progressBar;
    ToggleSwitch *outputToggle;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWidget window;
    window.show();
    
    return app.exec();
}

#include "pkg_tool_gui.moc"
