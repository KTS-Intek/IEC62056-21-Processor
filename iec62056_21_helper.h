#ifndef IEC62056_21_HELPER_H
#define IEC62056_21_HELPER_H

#include <QObject>

#include <QVariantHash>
#include <QDebug>

class IEC62056_21_Helper : public QObject
{
    Q_OBJECT
public:
    explicit IEC62056_21_Helper(QObject *parent = nullptr);

    struct OneIECParsedAnswer
    {
        QString value;
        QString units;//optional
        OneIECParsedAnswer() {}
        OneIECParsedAnswer(const QString &value, const QString &units) : value(value), units(units) {}
    };



    quint8 readNumber;

    bool verbouseMode;

    void insertEmptyMessageWithAheavyAnswer(QVariantHash &hashMessage);


    void insertEndSymb2_2903andMessage4obis(QVariantHash &hashMessage, const QString &obiscode);


    void insertEndSymb2_2903andMessage(QVariantHash &hashMessage, const QByteArray &messageh);
    void insertAnswr0_06andMessage(QVariantHash &hashMessage, const QByteArray &messageh);

    void insertEndSymb2_2903(QVariantHash &hashMessage);
    void insertAnswr0_06(QVariantHash &hashMessage);

    void insertMessage0(QVariantHash &hashMessage, const QByteArray &messageh);



    static bool isLogin2supportedMeterExt(const QVariantHash &hashRead, const QString &supportedMeters);

    static bool isLogin2supportedMeterExt(const QByteArray &readArr, const QString &supportedMeters);


    static bool isLoginGoodSmart(const QVariantHash &readHash, const quint16 &goodStep, QVariantHash &hashTmpData);

    static bool decodeMeterSNBase(const QVariantHash &hashRead, const QByteArray &hexLeftP, const quint16 &goodStep, QVariantHash &hashTmpData, bool &isSnBroken);


    static QString versionFromMessageExt(const QVariantHash &hashRead, const QByteArray &prefix, const int &len);

    static QString versionFromMessageExt(const QString &readArr, const int &len);



     QByteArray messageHexWithBCC(const QByteArray &writeArr);

     QByteArray messageWithBCC(const QByteArray &writeArr);

     QByteArray byteBCC(const QByteArray &arr);


     QByteArray message2meterChecks(QVariantHash &hashTmpData, QByteArray &lastAutoDetectNi, const QVariantHash &hashConstData, const QString &defVersion, const QDateTime &lastAutoDetectDt);

    bool isItYourReadMessageExt(const QByteArray &arr, const QString &supportedMetersPrefixes);


    bool getHashReadValue(const QVariantHash &hashRead, const QString &lastObis, const QByteArray &valueUnits, const int &prec, QString &valStr);


    bool getReadArrValue(const QByteArray &readArr, const QString &lastObis, const QByteArray &valueUnits, const int &prec, QString &valStr);

    int calculateEnrgIndxExt(qint16 currEnrg, const QStringList &listEnrg, const QStringList &spprtdListEnrg, const QStringList &listPlgEnrg);

    QByteArray getReadMessageFromObis(const QString &obiscode);


    QVariantHash getGoodByeMessage(QVariantHash &hashTmpData);

    QVariantHash getGoodByeMessageSmpl();


    QVariantHash getStep0HashMesssage(QVariantHash &hashTmpData, const QByteArray &arrNI);

    QVariantHash getStep1HashMesssageLowLevel();


    QVariantHash getStep1HashMesssage();

    QVariantHash getStep2HashMesssage(const QByteArray &passwordh);

    void markStep0Done(QVariantHash &hashTmpData);

    void markStep2Done(QVariantHash &hashTmpData, const quint16 &nextStep);

    bool isRead06(const QVariantHash &hashRead);
    bool isRead06arr(const QByteArray &arrRead);


    bool isReadThisPartly(const QVariantHash &hashRead, const QByteArray &startsWithHex);
    bool isReadThisPartly(const QByteArray &arrRead, const QByteArray &startsWithHex);


    QVariantHash getHelloMeter(const QVariantHash &hashMeterConstData);


    QHash<QString, OneIECParsedAnswer> getAnswersExt(const QVariantHash &hashRead);//it can be pretty long

    QHash<QString, OneIECParsedAnswer> getAnswers(const QByteArray &readArr);//it can be pretty long

    OneIECParsedAnswer getAnAnswer(const QString &oneLine, QString &obis);

};

#endif // IEC62056_21_HELPER_H
