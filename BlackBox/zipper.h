#include <QString>
class Zipper{

public:
    static void compressFolderToZip (const QString& folderPath, const QString& zipFilePath);
    static void uncompressZipToFolder(const QString& zipFilePath, const QString& outputFolderPath);
    static QString createFolderAndMoveFile(const QString& filePath);
    static void uncompressRarToFolder(const QString &rarFilePath, const QString &outputFolderPath);
};

