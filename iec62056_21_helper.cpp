#include "iec62056_21_helper.h"

///[!] type-converter
#include "src/base/convertatype.h"
#include "src/base/prettyvalues.h"

//------------------------------------------------------------------------------------------------------------------------

IEC62056_21_Helper::IEC62056_21_Helper(QObject *parent): QObject(parent)
{
    verbouseMode = false;
#ifdef Q_OS_LINUX
#ifdef __amd64
    verbouseMode = true;
#endif
#endif

    readNumber = 5;//R5

}
//------------------------------------------------------------------------------------------------------------------------
void IEC62056_21_Helper::insertEmptyMessageWithAheavyAnswer(QVariantHash &hashMessage)
{
    hashMessage.insert("message_0", "");//do not send anything, just wait for an answer
    hashMessage.insert("heavy_answer_0", true); //it is about 4K

    hashMessage.insert("endSymb2", QByteArray::fromHex("0D 0A 03"));//

}
//------------------------------------------------------------------------------------------------------------------------
void IEC62056_21_Helper::insertEndSymb2_2903andMessage4obis(QVariantHash &hashMessage, const QString &obiscode)
{
    if(verbouseMode) qDebug() << "CE303 obiscode" << obiscode;

    insertEndSymb2_2903(hashMessage);
    hashMessage.insert("message_0", getReadMessageFromObis(obiscode));
}
//------------------------------------------------------------------------------------------------------------------------
void IEC62056_21_Helper::insertEndSymb2_2903andMessage(QVariantHash &hashMessage, const QByteArray &messageh)
{
    insertEndSymb2_2903(hashMessage);
    insertMessage0(hashMessage, messageh);
}
//------------------------------------------------------------------------------------------------------------------------
void IEC62056_21_Helper::insertAnswr0_06andMessage(QVariantHash &hashMessage, const QByteArray &messageh)
{
    insertAnswr0_06(hashMessage);
    insertMessage0(hashMessage, messageh);
}
//------------------------------------------------------------------------------------------------------------------------
void IEC62056_21_Helper::insertEndSymb2_2903(QVariantHash &hashMessage)
{
    hashMessage.insert("endSymb2", QByteArray::fromHex("2903"));//.P0.().`

}
//------------------------------------------------------------------------------------------------------------------------
void IEC62056_21_Helper::insertAnswr0_06(QVariantHash &hashMessage)
{
    hashMessage.insert("answr_0", QByteArray::fromHex("06"));

}
//------------------------------------------------------------------------------------------------------------------------
void IEC62056_21_Helper::insertMessage0(QVariantHash &hashMessage, const QByteArray &messageh)
{
    hashMessage.insert("message_0", QByteArray::fromHex(messageh)); //.P1.().a

}
//------------------------------------------------------------------------------------------------------------------------
bool IEC62056_21_Helper::isLogin2supportedMeterExt(const QVariantHash &hashRead, const QString &supportedMeters)
{
    return (isLogin2supportedMeterExt(hashRead.value("readArr_1").toByteArray(), supportedMeters) || isLogin2supportedMeterExt(hashRead.value("readArr_0").toByteArray(), supportedMeters));

}

//------------------------------------------------------------------------------------------------------------------------

bool IEC62056_21_Helper::isLogin2supportedMeterExt(const QByteArray &readArr, const QString &supportedMeters)
{
    const QStringList l = supportedMeters.split(" ", QString::SkipEmptyParts);
    const QString strl = readArr.simplified();
    for(int i = 0, imax = l.size(); i < imax; i++){
        if(strl.startsWith(l.at(i)))
            return true;
    }


    return false; //(readArr.startsWith("/EGM5G3M") || readArr.startsWith("/EGM5G1M"));

}

//------------------------------------------------------------------------------------------------------------------------

bool IEC62056_21_Helper::isLoginGoodSmart(const QVariantHash &readHash, const quint16 &goodStep, QVariantHash &hashTmpData)
{
    const QByteArray readArr = QByteArray::fromHex("0150300228") ;

    if(readHash.value("readArr_0").toByteArray().startsWith(readArr)){

        //It has no lastMeterId
//        if(arrNI.isEmpty()){
//            const QByteArray lastMeterId = getLastMeterId(readHash.value("readArr_0").toByteArray().mid(readArr.length()));

//            if(!lastMeterId.isEmpty() && lastMeterId.length() < 21)
//                hashTmpData.insert("lastMeterId", lastMeterId);

//        }
        hashTmpData.insert("messFail", false);
        hashTmpData.insert("step", goodStep);
        return true;
    }
    hashTmpData.insert("step", (quint16)0);
    return false;
}

//------------------------------------------------------------------------------------------------------------------------

bool IEC62056_21_Helper::decodeMeterSNBase(const QVariantHash &hashRead, const QByteArray &hexLeftP, const quint16 &goodStep, QVariantHash &hashTmpData, bool &isSnBroken)
{
    isSnBroken = false;
    if(hashRead.value("readArr_0").toByteArray().startsWith(QByteArray::fromHex(hexLeftP))){ //.0.0.0(

        QString strSN = hashRead.value("readArr_0").toString().simplified().trimmed().split('(', QString::SkipEmptyParts).last();
        while(!strSN.isEmpty() && strSN.right(1) != ")")
            strSN.chop(1);
        if(strSN.right(1) == ")")
            strSN.chop(1);

        strSN = strSN.simplified().trimmed();

        bool oksn ;
        strSN.toLongLong(&oksn);

        if(!strSN.isEmpty() && strSN.length() < 33 && oksn){
            hashTmpData.insert("messFail", false);
            hashTmpData.insert("SN", strSN);
            hashTmpData.insert("step", goodStep);
            return true;
        }
        isSnBroken = true;
//        if(error_counter >= 0)
//            hashTmpData.insert(MeterPluginHelper::errWarnKey(error_counter, true),
//                           MeterPluginHelper::prettyMess(QString("!MeterSN.isValid: %1").arg(QString(strSN).simplified().trimmed()),
//                                                         PrettyValues::prettyHexDump(hashRead.value("readArr_0").toByteArray()), true, isSnBroken));


    }
    hashTmpData.insert("logined", false);
    hashTmpData.insert("step", (quint16)0);
    return false;
}

//------------------------------------------------------------------------------------------------------------------------

QString IEC62056_21_Helper::versionFromMessageExt(const QVariantHash &hashRead, const QByteArray &prefix, const int &len)
{
    // /EGM5G3M144160724V1
    return versionFromMessageExt(hashRead.value("readArr_1").toByteArray().startsWith(prefix) ? hashRead.value("readArr_1").toString() : hashRead.value("readArr_0").toString(), len);

}

//------------------------------------------------------------------------------------------------------------------------

QString IEC62056_21_Helper::versionFromMessageExt(const QString &readArr, const int &len)
{
    // /EGM5G3M144160724V1
    // /LGZ5ZMR110CR.K53
     if(readArr.contains("CE201.1v")){
         QString s = readArr.mid( readArr.indexOf("CE") + 2 ).simplified().trimmed();
         s.replace("201.1v", "201v");
         return s;
     }
     return readArr.mid(5, len);// readArr.indexOf("CE") + 2 ).simplified().trimmed();
}

//------------------------------------------------------------------------------------------------------------------------

QByteArray IEC62056_21_Helper::messageHexWithBCC(const QByteArray &writeArr)
{
    return messageWithBCC(QByteArray::fromHex(writeArr));

}

//------------------------------------------------------------------------------------------------------------------------

QByteArray IEC62056_21_Helper::messageWithBCC(const QByteArray &writeArr)
{
    return writeArr + byteBCC(writeArr);

}

//------------------------------------------------------------------------------------------------------------------------

QByteArray IEC62056_21_Helper::byteBCC(const QByteArray &arr)
{
    int pochatok = 0;

    const int iMax = arr.size();

    if(arr.mid(0,1) == QByteArray::fromHex("01") || arr.mid(0,1) == QByteArray::fromHex("02"))
        pochatok++;

    QByteArray arrBCC(1, 0x0);

    QBitArray bitXOR(8);

    for(int i = pochatok; i < iMax; i++){
        QBitArray bitArr(8);

        for(int b = 0; b < 8; ++b)
            bitArr.setBit(b, arr.at(i) & (1 << b));

        for(int iXOR = 0; iXOR < 8; ++iXOR){
            bitXOR.setBit(iXOR, bitXOR.at(iXOR) ^ bitArr.at(iXOR));
        }
    }

    for(int b = 0; b < 7; b++)
        arrBCC[0] = arrBCC.at(0) | ((bitXOR.at(b) ? 1 : 0) << (b%8));

    return arrBCC;
}

//------------------------------------------------------------------------------------------------------------------------

QByteArray IEC62056_21_Helper::message2meterChecks(QVariantHash &hashTmpData, QByteArray &lastAutoDetectNi, const QVariantHash &hashConstData, const QString &defVersion, const QDateTime &lastAutoDetectDt)
{
    if(hashTmpData.value("vrsn").toString().isEmpty() && !defVersion.isEmpty())
        hashTmpData.insert("vrsn", defVersion);

    if(lastAutoDetectNi == hashConstData.value("NI").toByteArray()){
        if(qAbs(lastAutoDetectDt.secsTo(QDateTime::currentDateTimeUtc())) < 30)
            hashTmpData.insert("CE303_BuvEkt", true);
        lastAutoDetectNi.clear();

    }

    QByteArray arrNI = hashConstData.value("NI", QByteArray()).toByteArray();
    if(arrNI.length() > 19 || !hashConstData.value("hardAddrsn", true).toBool())//hardAddrsn
        arrNI.clear();


    return arrNI;
}

bool IEC62056_21_Helper::isItYourReadMessageExt(const QByteArray &arr, const QString &supportedMetersPrefixes)
{

    if(arr.length() == 1){
        const QBitArray bitArr = ConvertAtype::byteArrayToBitArray(arr);
        int counter = 0;
        for(int i = 0; i < 8; i++){
            if(bitArr.at(i))
                counter++;
        }

        if(verbouseMode) qDebug() << "CE301 bitArr " << bitArr << counter;

        if((counter%2 == 0 && !bitArr.at(7)) || (counter%2 != 0 && bitArr.at(7)))
           return true;

    }

    const QByteArray readArr = ConvertAtype::convertData7ToData8(arr);

    if(isLogin2supportedMeterExt(readArr, supportedMetersPrefixes))// readArr.left(9).toUpper() == "/EKT"  && readArr.contains("CE30")){
        return true;


    //log in start1
    if(readArr.left(5) == QByteArray::fromHex("01 50 30 02 28") && readArr.right(3).left(3) == QByteArray::fromHex("2903"))
        return true;


    //write
    if(readArr == QByteArray::fromHex("06") || readArr == QByteArray::fromHex("15") )
        return true;


    //read serial number
    if(readArr.left(7) == QByteArray::fromHex("02 30 2E 30 2E 30 28"))
        return true;


    //read time
    if(readArr.left(7) == QByteArray::fromHex("0254494D455F28"))
        return true;


    //read date
    if(readArr.left(7) == QByteArray::fromHex("02444154455F28"))
        return true;


    //STAT
    if(readArr.left(7) == QByteArray::fromHex("02535441545F28")  ||
            readArr == QByteArray::fromHex("020303"))
        return true;


    //data not ready ERR12
    if(readArr.left(9) == QByteArray::fromHex("022845525231322903"))
        return true;

    //120
    if(readArr.left(7) == QByteArray::fromHex("02544156455228")  ||
            readArr.left(7) == QByteArray::fromHex("02475241504528")  ||
            readArr.left(7) == QByteArray::fromHex("02475241504928")  ||
            readArr.left(7) == QByteArray::fromHex("02475241514528")  ||
            readArr.left(7) == QByteArray::fromHex("02475241514928"))
        return true;


    //140
    if(readArr.left(7) == QByteArray::fromHex("02 31 2E 38 2E 54 28")  ||
            readArr.left(7) == QByteArray::fromHex("02 31 2E 38 2E 31 28")  ||
            readArr.left(7) == QByteArray::fromHex("02 31 2E 38 2E 32 28") ||
            readArr.left(7) == QByteArray::fromHex("02 31 2E 38 2E 33 28"))
        return true;


    //180
    if(readArr.left(7) == QByteArray::fromHex("02454E44504528")  ||
            readArr.left(7) == QByteArray::fromHex("02454E44504928")  ||
            readArr.left(7) == QByteArray::fromHex("02454E44514528")  ||
            readArr.left(7) == QByteArray::fromHex("02454E44514928"))
        return true;


    //220
    if(readArr.left(7) == QByteArray::fromHex("02454E4D504528") ||
            readArr.left(7) == QByteArray::fromHex("02454E4D504928")  ||
            readArr.left(7) == QByteArray::fromHex("02454E4D514528")  ||
            readArr.left(7) == QByteArray::fromHex("02454E4D514928"))
        return true;


    if(readArr.left(1) == QByteArray::fromHex("01") || readArr.left(1) == QByteArray::fromHex("02")){
        if(readArr.right(1) == QByteArray::fromHex("03"))
            return true;

        const QByteArray bccB = readArr.right(1);
        QByteArray readArr2(readArr);
          readArr2.chop(1);
        if(readArr2.right(1) == QByteArray::fromHex("03") && bccB == byteBCC(readArr2))
            return true;

    }

    //if(readArr.left(2) == QByteArray::fromHex("0D0A")){
    if(readArr.right(2).left(1) == QByteArray::fromHex("03")){


        QByteArray bccB = readArr.right(1);
        QByteArray readArr2(readArr);
          readArr2.chop(1);

        if(verbouseMode) qDebug() << "CE301 bcc " << bccB.toHex() << byteBCC(readArr2).toHex();

        if(/*readArr2.right(1) == QByteArray::fromHex("03") &&*/ readArr2.right(4).left(3) == ")\r\n"){

            if(verbouseMode){
                qDebug() << "CE30x Tak____________" << readArr2.left(3).toHex() << readArr2.right(3).toHex();
                qDebug() << ConvertAtype::convertData7ToData8(arr);
            }
            return true;
        }
    }else{
        if(verbouseMode) qDebug() << "CE30x " << readArr.right(2).left(1).toHex() << readArr.toHex();
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------

bool IEC62056_21_Helper::getHashReadValue(const QVariantHash &hashRead, const QString &lastObis, const QByteArray &valueUnits, const int &prec, QString &valStr)
{
    return  getReadArrValue(hashRead.value("readArr_0").toByteArray(), lastObis, valueUnits, prec, valStr);

}

//------------------------------------------------------------------------------------------------------------------------

bool IEC62056_21_Helper::getReadArrValue(const QByteArray &readArr, const QString &lastObis, const QByteArray &valueUnits, const int &prec, QString &valStr)
{
    const QByteArray obisarr = QByteArray::fromHex("02") + lastObis.toLocal8Bit();
//1.8.2*19(023835.433*kWh)<CR><LF>
    if(readArr.startsWith(obisarr)){

       const int leftDuzka = readArr.indexOf("(");//6
       const int rightDuzka = readArr.indexOf(")");//21

       if(leftDuzka > 0 && rightDuzka > leftDuzka){
           //.1.8.4(0000000.0*kWh).o
           //.73.7.0(1.00) power factor comes without units
           QByteArray arr = readArr.mid(leftDuzka + 1, rightDuzka - leftDuzka - 1);//0000000.0*kWh
           if(valueUnits.isEmpty() || arr.endsWith(valueUnits)){
               if(!valueUnits.isEmpty())
                   arr.chop(valueUnits.length() + 1);

               bool ok;
               const qreal val = arr.toDouble(&ok);
               valStr = PrettyValues::prettyNumber(val, prec);
               if(!ok)
                   valStr = QString("!arr value %1").arg(QString(arr));
               return ok;
           }
           valStr = QString("!units %1 %2").arg(QString(valueUnits)).arg(QString(valueUnits.isEmpty() ? arr : arr.right(valueUnits.length())));


       }else{
           valStr = QString("%1 - %2 () indexes ").arg(leftDuzka).arg(rightDuzka);
       }

    }else{
        valStr = "bad obis ";
        qDebug() << "decodeTotalEnergyMessages 02 31 2E 38 2E" << readArr.left(5).toHex() ;
    }
    return  false;
}

//------------------------------------------------------------------------------------------------------------------------

int IEC62056_21_Helper::calculateEnrgIndxExt(qint16 currEnrg, const QStringList &listEnrg, const QStringList &spprtdListEnrg, const QStringList &listPlgEnrg)
{
    //currEnrg індекс в списку опитування
//    const QStringList spprtdListEnrg = getSupportedEnrg(pollCode, version);
    // int energyIndex = (-1);//індекс енергії планіну до індексу опитування
    if(currEnrg < 0)
        currEnrg = 0;

    if(verbouseMode) qDebug() << listEnrg << currEnrg << spprtdListEnrg ;
    for(qint16 iMax = listEnrg.size(); currEnrg < iMax; currEnrg++){

        if(spprtdListEnrg.contains(listEnrg.at(currEnrg))){//енергія підтримувана лічильником
//            const QStringList listPlgEnrg = getSupportedEnrg(pollCode, "");//енергія підтримувана плагіном
            if(verbouseMode) qDebug() << "calculateEnrgIndx " << spprtdListEnrg.indexOf(listEnrg.at(currEnrg)) << listPlgEnrg.indexOf(listEnrg.at(currEnrg)) << listEnrg.at(currEnrg) << currEnrg;

            return listPlgEnrg.indexOf(listEnrg.at(currEnrg));
        }
    }
    if(verbouseMode) qDebug() << "CE303 decode energyIndex power -1" << currEnrg;
    return (-2);
}

//------------------------------------------------------------------------------------------------------------------------

QByteArray IEC62056_21_Helper::getReadMessageFromObis(const QString &obiscode)
{
    //"01 52 35 02 31 2E 38 2E 31 28 29 03 5F"
    const QByteArray writearr = QByteArray::fromHex("01 52") + QByteArray::number(readNumber) + QByteArray::fromHex("02")  + obiscode.toLocal8Bit() + QByteArray::fromHex("03");
    return  messageWithBCC(writearr);
}
//------------------------------------------------------------------------------------------------------------------------

QVariantHash IEC62056_21_Helper::getGoodByeMessage(QVariantHash &hashTmpData)
{

    hashTmpData.insert("logined", false);
    return getGoodByeMessageSmpl();

}
//------------------------------------------------------------------------------------------------------------------------

QVariantHash IEC62056_21_Helper::getGoodByeMessageSmpl()
{
    QVariantHash hashMessage;
    if(verbouseMode) qDebug() << "CE303 poll done, good bye meter" ;

    hashMessage.insert("message_0", QByteArray::fromHex("01 42 30 03 71"));//0142300375
    //                hashMessage.insert("multicast_0", true);
    hashMessage.insert("data7EPt", true);

    hashMessage.insert("ok_answer_0", true);
    return hashMessage;

}
//------------------------------------------------------------------------------------------------------------------------
QVariantHash IEC62056_21_Helper::getStep0HashMesssage(QVariantHash &hashTmpData, const QByteArray &arrNI)
{
    QVariantHash hashMessage;
    QByteArray writeArr = "/?" + arrNI + "!" + QByteArray::fromHex("0D0A");
    if(hashTmpData.value("CE303_BuvEkt", false).toBool()){//hashTmpData.insert("CE303_BuvEkt", true);

        hashMessage.insert("message_0", QByteArray::fromHex("0142300371"));//.B0.q
        hashMessage.insert("ok_answer_0", true);
        hashTmpData.insert("logined", false);
        hashMessage.insert("message_1", writeArr);
    }else
        hashMessage.insert("message_0", writeArr);

    hashMessage.insert("endSymb", QByteArray::fromHex("0D0A"));
    return hashMessage;
}

QVariantHash IEC62056_21_Helper::getStep1HashMesssageLowLevel()
{
    QVariantHash hashMessage;
    insertEndSymb2_2903andMessage(hashMessage, "06 30 35 30 0D 0A");
    return hashMessage;
}
//------------------------------------------------------------------------------------------------------------------------
QVariantHash IEC62056_21_Helper::getStep1HashMesssage()
{
    QVariantHash hashMessage;
    insertEndSymb2_2903andMessage(hashMessage, "06 30 35 31 0D 0A");
    return hashMessage;
//    hashMessage.insert("message_0", QByteArray::fromHex("06 30 35 31 0D 0A")); //.051 "063051310D0A")); //<ACK>051<CRLF> || 001
    //    hashMessage.insert("endSymb2", QByteArray::fromHex("2903"));//.P0.().`
}
//------------------------------------------------------------------------------------------------------------------------
QVariantHash IEC62056_21_Helper::getStep2HashMesssage(const QByteArray &passwordh)
{

    QVariantHash hashMessage;
    insertAnswr0_06andMessage(hashMessage, "01 50 31 02 28" + passwordh + " 29 03 61");
    return hashMessage;
//    hashMessage.insert("message_0", QByteArray::fromHex("01 50 31 02 28 29 03 61")); //.P1.().a
    //    hashMessage.insert("answr_0", QByteArray::fromHex("06"));
}
//------------------------------------------------------------------------------------------------------------------------
void IEC62056_21_Helper::markStep0Done(QVariantHash &hashTmpData)
{

    hashTmpData.insert("step", 1);
    hashTmpData.insert("messFail", false);
    hashTmpData.insert("CE303_BuvEkt", true);
}
//------------------------------------------------------------------------------------------------------------------------
void IEC62056_21_Helper::markStep2Done(QVariantHash &hashTmpData, const quint16 &nextStep)
{

    hashTmpData.insert("logined", true);
    hashTmpData.insert("step", nextStep);
    hashTmpData.insert("messFail", false);
}
//------------------------------------------------------------------------------------------------------------------------
bool IEC62056_21_Helper::isRead06(const QVariantHash &hashRead)
{
    return isRead06arr(hashRead.value("readArr_0").toByteArray());
}
//------------------------------------------------------------------------------------------------------------------------
bool IEC62056_21_Helper::isRead06arr(const QByteArray &arrRead)
{
    return  (arrRead == QByteArray::fromHex("06"));
}

//------------------------------------------------------------------------------------------------------------------------

bool IEC62056_21_Helper::isReadThisPartly(const QVariantHash &hashRead, const QByteArray &startsWithHex)
{
    return isReadThisPartly(hashRead.value("readArr_0").toByteArray(), startsWithHex);
}
//------------------------------------------------------------------------------------------------------------------------


bool IEC62056_21_Helper::isReadThisPartly(const QByteArray &arrRead, const QByteArray &startsWithHex)
{
    return arrRead.startsWith(QByteArray::fromHex(startsWithHex));// "02 39 36 2E 31 2E 32 28")
}

//------------------------------------------------------------------------------------------------------------------------

QVariantHash IEC62056_21_Helper::getHelloMeter(const QVariantHash &hashMeterConstData)
{

    QVariantHash hash;
    /*
     * bool data7EPt = hashMessage.value("data7EPt", false).toBool();
     * QByteArray endSymb = hashMessage.value("endSymb", QByteArray("\r\n")).toByteArray();
     * QByteArray currNI = hashMeterConstData.value("NI").toByteArray();
     * hashMessage.value("message")
*/

    bool ok;
    if(hashMeterConstData.value("NI").toULongLong(&ok) < 1 || !ok)
        return QVariantHash();

    QString arrNI = hashMeterConstData.value("NI").toString();

    if(!hashMeterConstData.value("hardAddrsn", true).toBool())//hardAddrsn
        arrNI.clear();


    const QByteArray dataWrite = QString("/?%1!\r\n").arg(arrNI).toLocal8Bit();

    qint32 readFromPortLen = 17 ;

    hash.insert("data7EPt", true);
    hash.insert("readLen", readFromPortLen);
    hash.insert("endSymb", "\r\n");
    hash.insert("message", dataWrite);
    ///2F 3F 32 35 30 30 36 30 38 21 0D 0A  ?2500608!
    /// 2F 45 47 4D 35 47 33 4D 31 34 34 31 36 30 37 3234 56 31 0D 0A   /EGM5G3M144160724V1
    return hash;
}


//------------------------------------------------------------------------------------------------------------------------


QHash<QString, IEC62056_21_Helper::OneIECParsedAnswer> IEC62056_21_Helper::getAnswersExt(const QVariantHash &hashRead)
{
    QHash<QString, IEC62056_21_Helper::OneIECParsedAnswer>  out;

    for(int i = 0, imax = 0xFF; i < imax; i++){
        const QString readArrKey = QString("readArr_%1").arg(i);

        if(!hashRead.contains(readArrKey))
            break;

         QHash<QString, IEC62056_21_Helper::OneIECParsedAnswer>  r = getAnswers(hashRead.value(readArrKey).toByteArray());

        if(r.isEmpty())
            continue;

        out.swap(r);

    }
    return out;
}

//------------------------------------------------------------------------------------------------------------------------

QHash<QString, IEC62056_21_Helper::OneIECParsedAnswer> IEC62056_21_Helper::getAnswers(const QByteArray &readArr)
{
    //it starts with
//    ttyUSB0     > 14:30:23.119 02 30 2E 39 2E 32 28 32 31 2D 30 35 2D 32 35 29 .0.9.2(21-05-25)

    //it ends with
    // 2E 30 30 30 2A 6B 57 68 29 0D 0A 21 0D 0A 03 08 .000*kWh)..! or something else

    QHash<QString, IEC62056_21_Helper::OneIECParsedAnswer>  out;

    const int startFrom = readArr.startsWith(QByteArray::fromHex("02")) ? 1 : 0;
    int endsAt = readArr.indexOf(QByteArray::fromHex("0D 0A 03"));
    if(startFrom > 0 && endsAt > startFrom)
        endsAt--;
    if(endsAt > 0)
        endsAt--;

    const QStringList listData = QString(readArr.mid(startFrom, endsAt)).split("\r\n");


    for(int i = 0, imax = listData.size(); i < imax; i++){

        QString obis;
        const OneIECParsedAnswer answer = getAnAnswer(listData.at(i), obis);
        if(obis.isEmpty())
            continue;
        out.insert(obis, answer);
    }


    return out;

}

//------------------------------------------------------------------------------------------------------------------------

IEC62056_21_Helper::OneIECParsedAnswer IEC62056_21_Helper::getAnAnswer(const QString &oneLine, QString &obis)
{
//    30 2E 39 2E 31 28 31 34 3A 34 30 3A 34 31 29 0D 0A 31 2E 38 2E 31 28 30 32 33 39 34 39 2E 36 35 37 2A 6B 57 68 29 0D 0A 31 2E 38 2E 32 28
//                                      ..0.9.1(14:40:41)..1.8.1(023949.657*kWh)..1.8.2(

    OneIECParsedAnswer answer;
    obis.clear();
    if(!oneLine.endsWith(")"))
        return answer;
    const int leftBraket = oneLine.indexOf("(");
    if(leftBraket < 2)
        return answer;//it is impossible

    obis = oneLine.left(leftBraket);

    answer.value = oneLine.mid(leftBraket + 1);
    answer.value.chop(1);//remove right braket

    if(answer.value.contains(QString("*"))){
        const QStringList lValueUnits = answer.value.split("*");

        if(lValueUnits.size() > 1){
            answer.value = lValueUnits.at(0);
            answer.units = lValueUnits.at(1);
        }
    }

    return answer;

}

//------------------------------------------------------------------------------------------------------------------------
