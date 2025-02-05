#include "qdebug.h"
#include "qdir.h"
#include "zipper.h"
#include "quazip.h"
#include "quazipfile.h"
#include "unrar.h"

void Zipper::compressFolderToZip(const QString& folderPath, const QString& zipFilePath) {
        QuaZip zip(zipFilePath);
        if (!zip.open(QuaZip::mdCreate)) {
            qWarning() << "Failed to create ZIP file:" << zipFilePath;
            return;
        }

        QDir directory(folderPath);
        QStringList fileList = directory.entryList(QDir::Files | QDir::NoDotAndDotDot);

        for (const QString& fileName : fileList) {
            QString filePath = directory.filePath(fileName);
            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly)) {
                qWarning() << "Failed to open file:" << filePath;
                continue;
            }

            QuaZipFile zipFile(&zip);
            if (!zipFile.open(QIODevice::WriteOnly, fileName)) {
                qWarning() << "Failed to open file in ZIP:" << fileName;
                continue;
            }

            QByteArray fileData = file.readAll();
            zipFile.write(fileData);
            zipFile.close();
            file.close();
        }

        zip.close();
    }

void Zipper::uncompressZipToFolder(const QString& zipFilePath, const QString& outputFolderPath) {
        QuaZip zip(zipFilePath);
        if (!zip.open(QuaZip::mdUnzip)) {
            qWarning() << "Failed to open ZIP file:" << zipFilePath;
            return;
        }

        QDir outputDir(outputFolderPath);
        if (!outputDir.exists() && !outputDir.mkpath(".")) {
            qWarning() << "Failed to create output directory:" << outputFolderPath;
            zip.close();
            return;
        }

        QuaZipFile zipFile(&zip);
        for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
            QString fileName = zip.getCurrentFileName();
            QString outputFilePath = outputDir.filePath(fileName);

            QFile outputFile(outputFilePath);
            if (!outputFile.open(QIODevice::WriteOnly)) {
                qWarning() << "Failed to open output file:" << outputFilePath;
                continue;
            }

            if (!zipFile.open(QIODevice::ReadOnly)) {
                qWarning() << "Failed to open file in ZIP:" << fileName;
                continue;
            }

            QByteArray fileData = zipFile.readAll();
            outputFile.write(fileData);
            zipFile.close();
            outputFile.close();
        }

        zip.close();
    }

QString Zipper::createFolderAndMoveFile(const QString& filePath) {

    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists()) {
        qWarning("File does not exist.");
        return QString();
    }

    QString fileName = fileInfo.fileName();
    QString fileDirPath = fileInfo.absolutePath();

    QString folderName = fileInfo.baseName();
    QString newFolderPath = fileDirPath + "/" + folderName;

    QDir dir;
    if (!dir.exists(newFolderPath)) {
        if (!dir.mkpath(newFolderPath)) {
            qWarning("Could not create folder.");
            return QString();
        }
    }

    QString newFilePath = newFolderPath + "/" + fileName;
    if (QFile::rename(filePath, newFilePath)) {
        qDebug("File moved successfully.");
        return newFolderPath;
    } else {
        qWarning("Could not move the file.");
        return QString();
    }
}


void Zipper::uncompressRarToFolder(const QString &rarFilePath, const QString &outputFolderPath)
{
    QDir dir;
    if (!dir.exists(outputFolderPath)) {
        if (!dir.mkpath(outputFolderPath)) {
            qWarning() << "Could not create folder";
            return;
        }
    }

    RAROpenArchiveDataEx archiveData = {0};
    QByteArray rarFilePathBytes = rarFilePath.toLocal8Bit();
    archiveData.ArcName = rarFilePathBytes.data();
    archiveData.OpenMode = RAR_OM_EXTRACT;

    HANDLE archive = RAROpenArchiveEx(&archiveData);
    if (archive == nullptr) {
        qWarning() << "Could not open archive (.rar)" << rarFilePath;
        return;
    }

    int result = 0;
    while (result == 0) {
        RARHeaderDataEx headerData;
        memset(&headerData, 0, sizeof(headerData));

        result = RARReadHeaderEx(archive, &headerData);
        if (result != 0) {
            break;
        }

        QString outputFilePath = outputFolderPath + QDir::separator();// + QString::fromLocal8Bit(headerData.FileName);
        QByteArray outputFilePathBytes = outputFilePath.toLocal8Bit();

        result = RARProcessFile(archive, RAR_EXTRACT, outputFilePathBytes.data(), nullptr);
        if (result != 0) {
            qWarning() << "File extraction error" << QString::fromLocal8Bit(headerData.FileName);
        }
    }

    RARCloseArchive(archive);

    if (result == ERAR_END_ARCHIVE) {
        qDebug() << "Archive successfully extracted to :" << outputFolderPath;
        return;
    } else {
        qWarning() << "File extraction error";
        return;
    }
}


