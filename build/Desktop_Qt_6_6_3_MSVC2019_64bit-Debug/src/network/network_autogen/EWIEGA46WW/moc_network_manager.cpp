/****************************************************************************
** Meta object code from reading C++ file 'network_manager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../src/network/network_manager.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'network_manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSNetworkManagerENDCLASS = QtMocHelpers::stringData(
    "NetworkManager",
    "messageReceived",
    "",
    "CoreMessage::Msg",
    "message",
    "heartbeatSent",
    "reconnectAttempt",
    "m_currentReconnectAttempts",
    "m_maxReconnectAttempts",
    "connectionEstablished",
    "connectionLost",
    "networkError",
    "error",
    "processMessageQueue",
    "onNetworkError",
    "QString&",
    "onConnectionEstablished",
    "onConnectionLost",
    "onSocketReadyRead",
    "onReconnectAttempt",
    "currentAttempt",
    "maxAttempts"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t {
    uint offsetsAndSizes[44];
    char stringdata0[15];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[8];
    char stringdata5[14];
    char stringdata6[17];
    char stringdata7[27];
    char stringdata8[23];
    char stringdata9[22];
    char stringdata10[15];
    char stringdata11[13];
    char stringdata12[6];
    char stringdata13[20];
    char stringdata14[15];
    char stringdata15[9];
    char stringdata16[24];
    char stringdata17[17];
    char stringdata18[18];
    char stringdata19[19];
    char stringdata20[15];
    char stringdata21[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t qt_meta_stringdata_CLASSNetworkManagerENDCLASS = {
    {
        QT_MOC_LITERAL(0, 14),  // "NetworkManager"
        QT_MOC_LITERAL(15, 15),  // "messageReceived"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 16),  // "CoreMessage::Msg"
        QT_MOC_LITERAL(49, 7),  // "message"
        QT_MOC_LITERAL(57, 13),  // "heartbeatSent"
        QT_MOC_LITERAL(71, 16),  // "reconnectAttempt"
        QT_MOC_LITERAL(88, 26),  // "m_currentReconnectAttempts"
        QT_MOC_LITERAL(115, 22),  // "m_maxReconnectAttempts"
        QT_MOC_LITERAL(138, 21),  // "connectionEstablished"
        QT_MOC_LITERAL(160, 14),  // "connectionLost"
        QT_MOC_LITERAL(175, 12),  // "networkError"
        QT_MOC_LITERAL(188, 5),  // "error"
        QT_MOC_LITERAL(194, 19),  // "processMessageQueue"
        QT_MOC_LITERAL(214, 14),  // "onNetworkError"
        QT_MOC_LITERAL(229, 8),  // "QString&"
        QT_MOC_LITERAL(238, 23),  // "onConnectionEstablished"
        QT_MOC_LITERAL(262, 16),  // "onConnectionLost"
        QT_MOC_LITERAL(279, 17),  // "onSocketReadyRead"
        QT_MOC_LITERAL(297, 18),  // "onReconnectAttempt"
        QT_MOC_LITERAL(316, 14),  // "currentAttempt"
        QT_MOC_LITERAL(331, 11)   // "maxAttempts"
    },
    "NetworkManager",
    "messageReceived",
    "",
    "CoreMessage::Msg",
    "message",
    "heartbeatSent",
    "reconnectAttempt",
    "m_currentReconnectAttempts",
    "m_maxReconnectAttempts",
    "connectionEstablished",
    "connectionLost",
    "networkError",
    "error",
    "processMessageQueue",
    "onNetworkError",
    "QString&",
    "onConnectionEstablished",
    "onConnectionLost",
    "onSocketReadyRead",
    "onReconnectAttempt",
    "currentAttempt",
    "maxAttempts"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSNetworkManagerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   86,    2, 0x06,    1 /* Public */,
       5,    0,   89,    2, 0x06,    3 /* Public */,
       6,    2,   90,    2, 0x06,    4 /* Public */,
       9,    0,   95,    2, 0x06,    7 /* Public */,
      10,    0,   96,    2, 0x06,    8 /* Public */,
      11,    1,   97,    2, 0x06,    9 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      13,    0,  100,    2, 0x08,   11 /* Private */,
      14,    1,  101,    2, 0x08,   12 /* Private */,
      16,    0,  104,    2, 0x08,   14 /* Private */,
      17,    0,  105,    2, 0x08,   15 /* Private */,
      18,    0,  106,    2, 0x08,   16 /* Private */,
      19,    2,  107,    2, 0x08,   17 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    7,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   20,   21,

       0        // eod
};

Q_CONSTINIT const QMetaObject NetworkManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSNetworkManagerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSNetworkManagerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSNetworkManagerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<NetworkManager, std::true_type>,
        // method 'messageReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const CoreMessage::Msg &, std::false_type>,
        // method 'heartbeatSent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reconnectAttempt'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'connectionEstablished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connectionLost'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'networkError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'processMessageQueue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onNetworkError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString &, std::false_type>,
        // method 'onConnectionEstablished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onConnectionLost'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSocketReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReconnectAttempt'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void NetworkManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NetworkManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->messageReceived((*reinterpret_cast< std::add_pointer_t<CoreMessage::Msg>>(_a[1]))); break;
        case 1: _t->heartbeatSent(); break;
        case 2: _t->reconnectAttempt((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->connectionEstablished(); break;
        case 4: _t->connectionLost(); break;
        case 5: _t->networkError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->processMessageQueue(); break;
        case 7: _t->onNetworkError((*reinterpret_cast< std::add_pointer_t<QString&>>(_a[1]))); break;
        case 8: _t->onConnectionEstablished(); break;
        case 9: _t->onConnectionLost(); break;
        case 10: _t->onSocketReadyRead(); break;
        case 11: _t->onReconnectAttempt((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NetworkManager::*)(const CoreMessage::Msg & );
            if (_t _q_method = &NetworkManager::messageReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::heartbeatSent; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(int , int );
            if (_t _q_method = &NetworkManager::reconnectAttempt; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::connectionEstablished; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)();
            if (_t _q_method = &NetworkManager::connectionLost; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (NetworkManager::*)(QString );
            if (_t _q_method = &NetworkManager::networkError; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject *NetworkManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSNetworkManagerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NetworkManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void NetworkManager::messageReceived(const CoreMessage::Msg & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NetworkManager::heartbeatSent()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void NetworkManager::reconnectAttempt(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NetworkManager::connectionEstablished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void NetworkManager::connectionLost()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void NetworkManager::networkError(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
