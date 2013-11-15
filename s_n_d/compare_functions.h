#ifndef COMPARE_FUNCTIONS_H
#define COMPARE_FUNCTIONS_H

// TODO: For MD5, Image and Byte-to-Byte compare only files with the same format

// Compare Method: "File MD5"
extern QPair<QString, QString> cm_md5_map(const QString &file);
extern void cm_md5_reduce(QString &result, const QString &intermediate);

// Compare Method: "Byte-to-Byte"
//extern void cm_btb(const QStringList &fileList);

// Compare Method: "Image Only skip Metadata"
extern QPair<QString, QString> cm_imageData_map(const QString &file);

// Compare Method: "Metadata Only Skip Image Data
//extern void cm_imageMetadata(const QStringList &fileList);

// Compare Method: "Color Histogtram"
extern int cm_histogram(int threshold, const QStringList &fileList);

extern void compareHistogram(const QStringList &fileList, int treshold);
#endif
