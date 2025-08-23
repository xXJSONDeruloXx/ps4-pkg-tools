// SPDX-License-Identifier: GPL-2.0-or-later
#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QThread>
#include <QTimer>
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
        setWindowTitle("ps4-pkg-tool GUI (minimal)");
        auto *layout = new QGridLayout(this);

        auto *pkgLabel = new QLabel("PKG File:");
        pkgEdit = new QLineEdit; pkgEdit->setPlaceholderText("Select a .pkg file or choose a directory below");
        auto *pkgBtn = new QPushButton("Browse...");
        connect(pkgBtn, &QPushButton::clicked, this, &MainWidget::pickPkgFile);

        auto *dirLabel = new QLabel("Directory Mode (find all .pkg):");
        dirEdit = new QLineEdit; dirEdit->setPlaceholderText("Select directory containing .pkg files");
        auto *dirBtn = new QPushButton("Browse Dir...");
        connect(dirBtn, &QPushButton::clicked, this, &MainWidget::pickPkgDir);

        auto *outLabel = new QLabel("Output Directory (optional):");
        outEdit = new QLineEdit; outEdit->setPlaceholderText("If empty, uses PKG parent or directory itself");
        auto *outBtn = new QPushButton("Browse Out...");
        connect(outBtn, &QPushButton::clicked, this, &MainWidget::pickOutDir);

        pkgList = new QListWidget; pkgList->setSelectionMode(QAbstractItemView::ExtendedSelection);
        pkgList->setMinimumHeight(140);

        metaLabel = new QLabel("Metadata: -");
        progressBar = new QProgressBar; progressBar->setMinimum(0); progressBar->setValue(0);

        auto *extractBtn = new QPushButton("Extract Selected / Current");
        connect(extractBtn, &QPushButton::clicked, this, &MainWidget::startExtraction);

        layout->addWidget(pkgLabel, 0,0); layout->addWidget(pkgEdit,0,1); layout->addWidget(pkgBtn,0,2);
        layout->addWidget(dirLabel,1,0); layout->addWidget(dirEdit,1,1); layout->addWidget(dirBtn,1,2);
        layout->addWidget(outLabel,2,0); layout->addWidget(outEdit,2,1); layout->addWidget(outBtn,2,2);
        layout->addWidget(new QLabel("Detected PKG Files:"),3,0,1,3);
        layout->addWidget(pkgList,4,0,1,3);
        layout->addWidget(metaLabel,5,0,1,3);
        layout->addWidget(progressBar,6,0,1,3);
        layout->addWidget(extractBtn,7,0,1,3);

        connect(pkgList, &QListWidget::currentRowChanged, this, &MainWidget::updateMetadataForCurrent);
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
        if (!item) { metaLabel->setText("Metadata: -"); return; }
        PkgMetadata meta; auto err = ReadPkgMetadata(item->text().toStdString(), meta);
        if (err) { metaLabel->setText("Metadata error: " + QString::fromStdString(*err)); return; }
        metaLabel->setText(QString("TitleID: %1 | Size: %2 bytes | Flags: %3 | Files: %4")
                               .arg(QString::fromStdString(meta.title_id))
                               .arg(meta.pkg_size)
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
        if (targets.isEmpty()) { QMessageBox::warning(this, "No PKG", "No PKG file selected."); return; }
        QString outBase = outEdit->text();
        progressBar->setValue(0);
        // Sequential extraction for simplicity
        extractNext(targets, 0, outBase);
    }
    void extractNext(const QStringList &targets, int index, const QString &outBase) {
        if (index >= targets.size()) { QMessageBox::information(this, "Done", "All extractions complete."); return; }
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
                QMessageBox::critical(this, "Extraction Error", err);
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
    MainWidget w; w.resize(800, 500); w.show();
    return app.exec();
}
