#ifndef IEC62056_21_HELPER_H
#define IEC62056_21_HELPER_H

#include <QVariantHash>

class IEC62056_21_Helper : public QObject
{
    Q_OBJECT
public:
    explicit IEC62056_21_Helper(QObject *parent = nullptr);


    bool verbouseMode;

    static bool isLogin2supportedMeterExt(const QVariantHash &hashRead, const QString &supportedMeters);

    static bool isLogin2supportedMeterExt(const QByteArray &readArr, const QString &supportedMeters);


    static bool isLoginGoodSmart(const QVariantHash &readHash, const quint16 &goodStep, QVariantHash &hashTmpData);

    static bool decodeMeterSNBase(const QVariantHash &hashRead, const QByteArray &hexLeftP, const quint16 &goodStep, QVariantHash &hashTmpData, bool &isSnBroken);


    static QString versionFromMessageExt(const QVariantHash &hashRead, const QByteArray &prefix, const int &len);

    static QString versionFromMessageExt(const QString &readArr, const int &len);



    static QByteArray messageHexWithBCC(const QByteArray &writeArr);

    static QByteArray messageWithBCC(const QByteArray &writeArr);

    static QByteArray byteBCC(const QByteArray &arr);


    static QByteArray message2meterChecks(QVariantHash &hashTmpData, QByteArray &lastAutoDetectNi, const QVariantHash &hashConstData, const QString &defVersion, const QDateTime &lastAutoDetectDt);

    bool isItYourReadMessage(const QByteArray &arr);


    bool getHashReadValue(const QVariantHash &hashRead, const QString &lastObis, const QByteArray &valueUnits, const int &prec, QString &valStr);


    bool getReadArrValue(const QByteArray &readArr, const QString &lastObis, const QByteArray &valueUnits, const int &prec, QString &valStr);

    int calculateEnrgIndxExt(qint16 currEnrg, const quint8 &pollCode, const QStringList &listEnrg, const QStringList &spprtdListEnrg, const QStringList &listPlgEnrg);

    QByteArray getReadMessageFromObis(const QString &obiscode);

};

#endif // IEC62056_21_HELPER_H
