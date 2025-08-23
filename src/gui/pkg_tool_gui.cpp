// SPDX-License-Identifier: GPL-2.0-or-later
#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QThread>
#include <QTimer>
#include <QFrame>
#include <QSizePolicy>
#include <atomic>
#include <filesystem>
#include <optional>
#include <thread>
#include "pkg_tool/lib.h"

class ExtractWorker : public QObject {
    Q_OBJECT
public:
    ExtractWorker(QString pkg, QString out) : m_pkg(std::move(pkg)), m_out(std::move(out)) {}
signals:
    void progress(uint32_t extracted, uint32_t total);
    void finished(QString errorMsg);
public slots:
    void run() {
        std::filesystem::path pkg_path = m_pkg.toStdString();
        std::filesystem::path out_dir = m_out.toStdString();
        auto err = ExtractPkg(pkg_path, out_dir, {}, [&](uint32_t e, uint32_t t) { emit progress(e, t); });
        emit finished(err ? QString::fromStdString(*err) : QString());
    }
private:
    QString m_pkg;
    QString m_out;
};

class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget() {
        setWindowTitle("PS4 PKG Tool - Modern GUI");
        setMinimumSize(900, 650);
        setupUI();
        applyDarkTheme();
    }

private:
    void setupUI() {
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(15);
        mainLayout->setContentsMargins(20, 20, 20, 20);

        // Title
        auto *titleLabel = new QLabel("PS4 PKG Extraction Tool");
        titleLabel->setObjectName("titleLabel");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Input section
        auto *inputGroup = new QGroupBox("Input Selection");
        inputGroup->setObjectName("groupBox");
        auto *inputLayout = new QVBoxLayout(inputGroup);
        
        // PKG File selection
        auto *pkgLayout = new QHBoxLayout;
        auto *pkgLabel = new QLabel("PKG File:");
        pkgLabel->setMinimumWidth(120);
        pkgEdit = new QLineEdit;
        pkgEdit->setPlaceholderText("Select a .pkg file or choose a directory below");
        auto *pkgBtn = new QPushButton("Browse...");
        pkgBtn->setObjectName("primaryButton");
        connect(pkgBtn, &QPushButton::clicked, this, &MainWidget::pickPkgFile);
        
        pkgLayout->addWidget(pkgLabel);
        pkgLayout->addWidget(pkgEdit);
        pkgLayout->addWidget(pkgBtn);
        inputLayout->addLayout(pkgLayout);

        // Directory selection
        auto *dirLayout = new QHBoxLayout;
        auto *dirLabel = new QLabel("Directory Mode:");
        dirLabel->setMinimumWidth(120);
        dirEdit = new QLineEdit;
        dirEdit->setPlaceholderText("Select directory containing .pkg files");
        auto *dirBtn = new QPushButton("Browse Dir...");
        dirBtn->setObjectName("primaryButton");
        connect(dirBtn, &QPushButton::clicked, this, &MainWidget::pickPkgDir);
        
        dirLayout->addWidget(dirLabel);
        dirLayout->addWidget(dirEdit);
        dirLayout->addWidget(dirBtn);
        inputLayout->addLayout(dirLayout);

        // Output directory selection
        auto *outLayout = new QHBoxLayout;
        auto *outLabel = new QLabel("Output Directory:");
        outLabel->setMinimumWidth(120);
        outEdit = new QLineEdit;
        outEdit->setPlaceholderText("If empty, uses PKG parent or directory itself");
        auto *outBtn = new QPushButton("Browse Out...");
        outBtn->setObjectName("primaryButton");
        connect(outBtn, &QPushButton::clicked, this, &MainWidget::pickOutDir);
        
        outLayout->addWidget(outLabel);
        outLayout->addWidget(outEdit);
        outLayout->addWidget(outBtn);
        inputLayout->addLayout(outLayout);

        mainLayout->addWidget(inputGroup);

        // PKG List section
        auto *listGroup = new QGroupBox("Detected PKG Files");
        listGroup->setObjectName("groupBox");
        auto *listLayout = new QVBoxLayout(listGroup);
        
        pkgList = new QListWidget;
        pkgList->setSelectionMode(QAbstractItemView::ExtendedSelection);
        pkgList->setMinimumHeight(200);
        pkgList->setObjectName("pkgList");
        listLayout->addWidget(pkgList);

        mainLayout->addWidget(listGroup);

        // Metadata section
        auto *metaGroup = new QGroupBox("Package Information");
        metaGroup->setObjectName("groupBox");
        auto *metaLayout = new QVBoxLayout(metaGroup);
        
        metaLabel = new QLabel("Select a PKG file to view metadata");
        metaLabel->setObjectName("metaLabel");
        metaLabel->setWordWrap(true);
        metaLayout->addWidget(metaLabel);

        mainLayout->addWidget(metaGroup);

        // Progress section
        auto *progressGroup = new QGroupBox("Extraction Progress");
        progressGroup->setObjectName("groupBox");
        auto *progressLayout = new QVBoxLayout(progressGroup);
        
        progressBar = new QProgressBar;
        progressBar->setMinimum(0);
        progressBar->setValue(0);
        progressBar->setObjectName("progressBar");
        progressBar->setTextVisible(true);
        progressLayout->addWidget(progressBar);

        mainLayout->addWidget(progressGroup);

        // Action buttons
        auto *buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch();
        
        auto *extractBtn = new QPushButton("🚀 Extract Selected Packages");
        extractBtn->setObjectName("extractButton");
        extractBtn->setMinimumHeight(50);
        extractBtn->setMinimumWidth(250);
        connect(extractBtn, &QPushButton::clicked, this, &MainWidget::startExtraction);
        
        buttonLayout->addWidget(extractBtn);
        buttonLayout->addStretch();
        
        mainLayout->addLayout(buttonLayout);

        connect(pkgList, &QListWidget::currentRowChanged, this, &MainWidget::updateMetadataForCurrent);
    }

    void applyDarkTheme() {
        setStyleSheet(R"(
            QWidget {
                background-color: #2b2b2b;
                color: #ffffff;
                font-family: 'Segoe UI', 'Ubuntu', sans-serif;
                font-size: 11px;
            }
            
            #titleLabel {
                font-size: 24px;
                font-weight: bold;
                color: #4CAF50;
                margin: 10px 0px 20px 0px;
                padding: 10px;
            }
            
            QGroupBox {
                font-weight: bold;
                border: 2px solid #555555;
                border-radius: 8px;
                margin: 5px 0px;
                padding-top: 15px;
                background-color: #323232;
            }
            
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0px 8px 0px 8px;
                color: #4CAF50;
                font-size: 12px;
            }
            
            QLineEdit {
                background-color: #404040;
                border: 2px solid #555555;
                border-radius: 6px;
                padding: 8px;
                font-size: 11px;
                selection-background-color: #4CAF50;
            }
            
            QLineEdit:focus {
                border-color: #4CAF50;
                background-color: #454545;
            }
            
            QPushButton {
                background-color: #404040;
                border: 2px solid #555555;
                border-radius: 6px;
                padding: 8px 16px;
                font-weight: bold;
                min-width: 80px;
            }
            
            QPushButton:hover {
                background-color: #4a4a4a;
                border-color: #666666;
            }
            
            QPushButton:pressed {
                background-color: #353535;
            }
            
            #primaryButton {
                background-color: #2196F3;
                border-color: #1976D2;
                color: white;
            }
            
            #primaryButton:hover {
                background-color: #1E88E5;
                border-color: #1565C0;
            }
            
            #primaryButton:pressed {
                background-color: #1565C0;
            }
            
            #extractButton {
                background-color: #4CAF50;
                border-color: #388E3C;
                color: white;
                font-size: 14px;
                font-weight: bold;
            }
            
            #extractButton:hover {
                background-color: #45a049;
                border-color: #2E7D32;
            }
            
            #extractButton:pressed {
                background-color: #388E3C;
            }
            
            QListWidget {
                background-color: #404040;
                border: 2px solid #555555;
                border-radius: 6px;
                padding: 5px;
                selection-background-color: #4CAF50;
                selection-color: white;
                alternate-background-color: #454545;
            }
            
            QListWidget::item {
                padding: 8px;
                border-bottom: 1px solid #555555;
                border-radius: 3px;
                margin: 1px;
            }
            
            QListWidget::item:hover {
                background-color: #4a4a4a;
            }
            
            QListWidget::item:selected {
                background-color: #4CAF50;
                color: white;
            }
            
            #metaLabel {
                background-color: #404040;
                border: 2px solid #555555;
                border-radius: 6px;
                padding: 12px;
                font-family: 'Consolas', 'Monaco', monospace;
                font-size: 10px;
                color: #E0E0E0;
            }
            
            QProgressBar {
                background-color: #404040;
                border: 2px solid #555555;
                border-radius: 6px;
                height: 25px;
                text-align: center;
                font-weight: bold;
            }
            
            QProgressBar::chunk {
                background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #4CAF50, stop:1 #45a049);
                border-radius: 4px;
            }
            
            QLabel {
                color: #E0E0E0;
            }
        )");
    }

private slots:
    void pickPkgFile() {
        QString file = QFileDialog::getOpenFileName(this, "Select PKG", QString(), "PKG Files (*.pkg)");
        if (file.isEmpty()) return;
        pkgEdit->setText(file);
        pkgList->clear();
        pkgList->addItem(file);
        updateMetadataForCurrent();
    }
    void pickPkgDir() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Directory with PKG files");
        if (dir.isEmpty()) return;
        dirEdit->setText(dir);
        pkgList->clear();
        for (auto &p : ListPkgFiles(dir.toStdString(), false)) {
            pkgList->addItem(QString::fromStdString(p.string()));
        }
        if (pkgList->count() > 0) {
            pkgList->setCurrentRow(0);
            updateMetadataForCurrent();
        }
    }
    void pickOutDir() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Output Directory");
        if (dir.isEmpty()) return;
        outEdit->setText(dir);
    }
    void updateMetadataForCurrent() {
        auto *item = pkgList->currentItem();
        if (!item) { 
            metaLabel->setText("Select a PKG file to view metadata"); 
            return; 
        }
        PkgMetadata meta; 
        auto err = ReadPkgMetadata(item->text().toStdString(), meta);
        if (err) { 
            metaLabel->setText("❌ Metadata error: " + QString::fromStdString(*err)); 
            return; 
        }
        metaLabel->setText(QString("📦 Title ID: %1\n"
                                   "📏 Size: %2 bytes (%3 MB)\n"
                                   "🏷️ Flags: %4\n"
                                   "📁 Files: %5")
                               .arg(QString::fromStdString(meta.title_id))
                               .arg(meta.pkg_size)
                               .arg(QString::number(meta.pkg_size / (1024.0 * 1024.0), 'f', 1))
                               .arg(meta.flags)
                               .arg(meta.file_count));
    }
    void startExtraction() {
        QStringList targets;
        auto selected = pkgList->selectedItems();
        if (!selected.isEmpty()) {
            for (auto *i : selected) targets << i->text();
        } else if (pkgList->count() == 1) {
            targets << pkgList->item(0)->text();
        } else if (!pkgEdit->text().isEmpty()) {
            targets << pkgEdit->text();
        }
        if (targets.isEmpty()) { 
            QMessageBox::warning(this, "No PKG Selected", "Please select at least one PKG file to extract."); 
            return; 
        }
        QString outBase = outEdit->text();
        progressBar->setValue(0);
        progressBar->setFormat("Preparing extraction...");
        // Sequential extraction for simplicity
        extractNext(targets, 0, outBase);
    }
    void extractNext(const QStringList &targets, int index, const QString &outBase) {
        if (index >= targets.size()) { 
            progressBar->setFormat("✅ All extractions complete!");
            QMessageBox::information(this, "Extraction Complete", "All PKG files have been successfully extracted!"); 
            return; 
        }
        QString pkgPath = targets[index];
        // Determine output directory
        QString outDir = outBase;
        if (outDir.isEmpty()) {
            std::filesystem::path p = pkgPath.toStdString();
            outDir = QString::fromStdString(p.parent_path().string());
        }
        // Read metadata to build subdir
        PkgMetadata meta; ReadPkgMetadata(pkgPath.toStdString(), meta);
        std::filesystem::path sub = std::filesystem::path(outDir.toStdString()) / meta.title_id;
        QString finalOut = QString::fromStdString(sub.string());

        progressBar->setFormat(QString("Extracting %1 (%2/%3)...")
                              .arg(QString::fromStdString(meta.title_id))
                              .arg(index + 1)
                              .arg(targets.size()));

        // Threaded extraction
        auto *worker = new ExtractWorker(pkgPath, finalOut);
        auto *thread = new QThread; worker->moveToThread(thread);
        connect(thread, &QThread::started, worker, &ExtractWorker::run);
        connect(worker, &ExtractWorker::progress, this, [this](uint32_t e, uint32_t t){
            progressBar->setMaximum(static_cast<int>(t));
            progressBar->setValue(static_cast<int>(e));
        });
        connect(worker, &ExtractWorker::finished, this, [=](QString err){
            thread->quit(); thread->wait(); worker->deleteLater(); thread->deleteLater();
            if (!err.isEmpty()) {
                progressBar->setFormat("❌ Extraction failed!");
                QMessageBox::critical(this, "Extraction Error", 
                    QString("Failed to extract package:\n\n%1").arg(err));
            }
            progressBar->setValue(0);
            extractNext(targets, index+1, outBase);
        });
        thread->start();
    }
private:
    QLineEdit *pkgEdit{}; QLineEdit *dirEdit{}; QLineEdit *outEdit{};
    QListWidget *pkgList{}; QLabel *metaLabel{}; QProgressBar *progressBar{};
};

#include "pkg_tool_gui.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("PS4 PKG Tool");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("PS4 PKG Tools");
    
    MainWidget w; 
    w.resize(900, 650); 
    w.show();
    return app.exec();
}
