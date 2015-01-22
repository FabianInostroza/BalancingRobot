/****************************************************************************
** Meta object code from reading C++ file 'qserialport.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../qtserialport/src/serialport/qserialport.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qserialport.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSerialPort[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       8,   64, // properties
       8,   96, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   13,   12,   12, 0x05,
      90,   81,   12,   12, 0x05,
     136,  129,   12,   12, 0x05,
     180,  171,   12,   12, 0x05,
     231,  219,   12,   12, 0x05,
     283,  276,   12,   12, 0x05,
     340,  336,   12,   12, 0x05,
     371,  336,   12,   12, 0x05,
     414,  398,   12,   12, 0x05,
     458,  450,   12,   12, 0x05,

 // properties: name, type, flags
     502,  495, 0x02495103,
      81,  511, 0x0049510b,
     129,  520, 0x0049510b,
     171,  527, 0x0049510b,
     219,  536, 0x0049510b,
     553,  548, 0x01495103,
     571,  548, 0x01495103,
     601,  585, 0x0049500d,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,
       6,
       7,
       8,

 // enums: name, flags, count, data
     607, 0x1,    3,  128,
     618, 0x0,    9,  134,
     511, 0x0,    5,  152,
     520, 0x0,    6,  162,
     527, 0x0,    4,  174,
     536, 0x0,    4,  182,
     627, 0x1,   11,  190,
     585, 0x0,   14,  212,

 // enum data: key, value
     641, uint(QSerialPort::Input),
     647, uint(QSerialPort::Output),
     654, uint(QSerialPort::AllDirections),
     668, uint(QSerialPort::Baud1200),
     677, uint(QSerialPort::Baud2400),
     686, uint(QSerialPort::Baud4800),
     695, uint(QSerialPort::Baud9600),
     704, uint(QSerialPort::Baud19200),
     714, uint(QSerialPort::Baud38400),
     724, uint(QSerialPort::Baud57600),
     734, uint(QSerialPort::Baud115200),
     745, uint(QSerialPort::UnknownBaud),
     757, uint(QSerialPort::Data5),
     763, uint(QSerialPort::Data6),
     769, uint(QSerialPort::Data7),
     775, uint(QSerialPort::Data8),
     781, uint(QSerialPort::UnknownDataBits),
     797, uint(QSerialPort::NoParity),
     806, uint(QSerialPort::EvenParity),
     817, uint(QSerialPort::OddParity),
     827, uint(QSerialPort::SpaceParity),
     839, uint(QSerialPort::MarkParity),
     850, uint(QSerialPort::UnknownParity),
     864, uint(QSerialPort::OneStop),
     872, uint(QSerialPort::OneAndHalfStop),
     887, uint(QSerialPort::TwoStop),
     895, uint(QSerialPort::UnknownStopBits),
     911, uint(QSerialPort::NoFlowControl),
     925, uint(QSerialPort::HardwareControl),
     941, uint(QSerialPort::SoftwareControl),
     957, uint(QSerialPort::UnknownFlowControl),
     976, uint(QSerialPort::NoSignal),
     985, uint(QSerialPort::TransmittedDataSignal),
    1007, uint(QSerialPort::ReceivedDataSignal),
    1026, uint(QSerialPort::DataTerminalReadySignal),
    1050, uint(QSerialPort::DataCarrierDetectSignal),
    1074, uint(QSerialPort::DataSetReadySignal),
    1093, uint(QSerialPort::RingIndicatorSignal),
    1113, uint(QSerialPort::RequestToSendSignal),
    1133, uint(QSerialPort::ClearToSendSignal),
    1151, uint(QSerialPort::SecondaryTransmittedDataSignal),
    1182, uint(QSerialPort::SecondaryReceivedDataSignal),
    1210, uint(QSerialPort::NoError),
    1218, uint(QSerialPort::DeviceNotFoundError),
    1238, uint(QSerialPort::PermissionError),
    1254, uint(QSerialPort::OpenError),
    1264, uint(QSerialPort::ParityError),
    1276, uint(QSerialPort::FramingError),
    1289, uint(QSerialPort::BreakConditionError),
    1309, uint(QSerialPort::WriteError),
    1320, uint(QSerialPort::ReadError),
    1330, uint(QSerialPort::ResourceError),
    1344, uint(QSerialPort::UnsupportedOperationError),
    1370, uint(QSerialPort::UnknownError),
    1383, uint(QSerialPort::TimeoutError),
    1396, uint(QSerialPort::NotOpenError),

       0        // eod
};

static const char qt_meta_stringdata_QSerialPort[] = {
    "QSerialPort\0\0baudRate,directions\0"
    "baudRateChanged(qint32,QSerialPort::Directions)\0"
    "dataBits\0dataBitsChanged(QSerialPort::DataBits)\0"
    "parity\0parityChanged(QSerialPort::Parity)\0"
    "stopBits\0stopBitsChanged(QSerialPort::StopBits)\0"
    "flowControl\0flowControlChanged(QSerialPort::FlowControl)\0"
    "policy\0dataErrorPolicyChanged(QSerialPort::DataErrorPolicy)\0"
    "set\0dataTerminalReadyChanged(bool)\0"
    "requestToSendChanged(bool)\0serialPortError\0"
    "error(QSerialPort::SerialPortError)\0"
    "restore\0settingsRestoredOnCloseChanged(bool)\0"
    "qint32\0baudRate\0DataBits\0Parity\0"
    "StopBits\0FlowControl\0bool\0dataTerminalReady\0"
    "requestToSend\0SerialPortError\0error\0"
    "Directions\0BaudRate\0PinoutSignals\0"
    "Input\0Output\0AllDirections\0Baud1200\0"
    "Baud2400\0Baud4800\0Baud9600\0Baud19200\0"
    "Baud38400\0Baud57600\0Baud115200\0"
    "UnknownBaud\0Data5\0Data6\0Data7\0Data8\0"
    "UnknownDataBits\0NoParity\0EvenParity\0"
    "OddParity\0SpaceParity\0MarkParity\0"
    "UnknownParity\0OneStop\0OneAndHalfStop\0"
    "TwoStop\0UnknownStopBits\0NoFlowControl\0"
    "HardwareControl\0SoftwareControl\0"
    "UnknownFlowControl\0NoSignal\0"
    "TransmittedDataSignal\0ReceivedDataSignal\0"
    "DataTerminalReadySignal\0DataCarrierDetectSignal\0"
    "DataSetReadySignal\0RingIndicatorSignal\0"
    "RequestToSendSignal\0ClearToSendSignal\0"
    "SecondaryTransmittedDataSignal\0"
    "SecondaryReceivedDataSignal\0NoError\0"
    "DeviceNotFoundError\0PermissionError\0"
    "OpenError\0ParityError\0FramingError\0"
    "BreakConditionError\0WriteError\0ReadError\0"
    "ResourceError\0UnsupportedOperationError\0"
    "UnknownError\0TimeoutError\0NotOpenError\0"
};

void QSerialPort::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QSerialPort *_t = static_cast<QSerialPort *>(_o);
        switch (_id) {
        case 0: _t->baudRateChanged((*reinterpret_cast< qint32(*)>(_a[1])),(*reinterpret_cast< QSerialPort::Directions(*)>(_a[2]))); break;
        case 1: _t->dataBitsChanged((*reinterpret_cast< QSerialPort::DataBits(*)>(_a[1]))); break;
        case 2: _t->parityChanged((*reinterpret_cast< QSerialPort::Parity(*)>(_a[1]))); break;
        case 3: _t->stopBitsChanged((*reinterpret_cast< QSerialPort::StopBits(*)>(_a[1]))); break;
        case 4: _t->flowControlChanged((*reinterpret_cast< QSerialPort::FlowControl(*)>(_a[1]))); break;
        case 5: _t->dataErrorPolicyChanged((*reinterpret_cast< QSerialPort::DataErrorPolicy(*)>(_a[1]))); break;
        case 6: _t->dataTerminalReadyChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->requestToSendChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->error((*reinterpret_cast< QSerialPort::SerialPortError(*)>(_a[1]))); break;
        case 9: _t->settingsRestoredOnCloseChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QSerialPort::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QSerialPort::staticMetaObject = {
    { &QIODevice::staticMetaObject, qt_meta_stringdata_QSerialPort,
      qt_meta_data_QSerialPort, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QSerialPort::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QSerialPort::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QSerialPort::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSerialPort))
        return static_cast<void*>(const_cast< QSerialPort*>(this));
    return QIODevice::qt_metacast(_clname);
}

int QSerialPort::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QIODevice::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< qint32*>(_v) = baudRate(); break;
        case 1: *reinterpret_cast< DataBits*>(_v) = dataBits(); break;
        case 2: *reinterpret_cast< Parity*>(_v) = parity(); break;
        case 3: *reinterpret_cast< StopBits*>(_v) = stopBits(); break;
        case 4: *reinterpret_cast< FlowControl*>(_v) = flowControl(); break;
        case 5: *reinterpret_cast< bool*>(_v) = isDataTerminalReady(); break;
        case 6: *reinterpret_cast< bool*>(_v) = isRequestToSend(); break;
        case 7: *reinterpret_cast< SerialPortError*>(_v) = error(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBaudRate(*reinterpret_cast< qint32*>(_v)); break;
        case 1: setDataBits(*reinterpret_cast< DataBits*>(_v)); break;
        case 2: setParity(*reinterpret_cast< Parity*>(_v)); break;
        case 3: setStopBits(*reinterpret_cast< StopBits*>(_v)); break;
        case 4: setFlowControl(*reinterpret_cast< FlowControl*>(_v)); break;
        case 5: setDataTerminalReady(*reinterpret_cast< bool*>(_v)); break;
        case 6: setRequestToSend(*reinterpret_cast< bool*>(_v)); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::ResetProperty) {
        switch (_id) {
        case 7: clearError(); break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QSerialPort::baudRateChanged(qint32 _t1, QSerialPort::Directions _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QSerialPort::dataBitsChanged(QSerialPort::DataBits _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QSerialPort::parityChanged(QSerialPort::Parity _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QSerialPort::stopBitsChanged(QSerialPort::StopBits _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QSerialPort::flowControlChanged(QSerialPort::FlowControl _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QSerialPort::dataErrorPolicyChanged(QSerialPort::DataErrorPolicy _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QSerialPort::dataTerminalReadyChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QSerialPort::requestToSendChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QSerialPort::error(QSerialPort::SerialPortError _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QSerialPort::settingsRestoredOnCloseChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_END_MOC_NAMESPACE
