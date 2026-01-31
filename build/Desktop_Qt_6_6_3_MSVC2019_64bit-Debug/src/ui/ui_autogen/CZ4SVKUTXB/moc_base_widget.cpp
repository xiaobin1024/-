/****************************************************************************
** Meta object code from reading C++ file 'base_widget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../src/ui/base/base_widget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'base_widget.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSBaseWidgetENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSBaseWidgetENDCLASS = QtMocHelpers::stringData(
    "BaseWidget",
    "pageEvent",
    "",
    "PageEvent",
    "event",
    "data",
    "navigateRequested",
    "pageName",
    "systemFunctionRequested",
    "SystemFunction",
    "function",
    "logoutRequested",
    "deleteAccountRequested",
    "themeToggleRequested"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSBaseWidgetENDCLASS_t {
    uint offsetsAndSizes[28];
    char stringdata0[11];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[6];
    char stringdata5[5];
    char stringdata6[18];
    char stringdata7[9];
    char stringdata8[24];
    char stringdata9[15];
    char stringdata10[9];
    char stringdata11[16];
    char stringdata12[23];
    char stringdata13[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSBaseWidgetENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSBaseWidgetENDCLASS_t qt_meta_stringdata_CLASSBaseWidgetENDCLASS = {
    {
        QT_MOC_LITERAL(0, 10),  // "BaseWidget"
        QT_MOC_LITERAL(11, 9),  // "pageEvent"
        QT_MOC_LITERAL(21, 0),  // ""
        QT_MOC_LITERAL(22, 9),  // "PageEvent"
        QT_MOC_LITERAL(32, 5),  // "event"
        QT_MOC_LITERAL(38, 4),  // "data"
        QT_MOC_LITERAL(43, 17),  // "navigateRequested"
        QT_MOC_LITERAL(61, 8),  // "pageName"
        QT_MOC_LITERAL(70, 23),  // "systemFunctionRequested"
        QT_MOC_LITERAL(94, 14),  // "SystemFunction"
        QT_MOC_LITERAL(109, 8),  // "function"
        QT_MOC_LITERAL(118, 15),  // "logoutRequested"
        QT_MOC_LITERAL(134, 22),  // "deleteAccountRequested"
        QT_MOC_LITERAL(157, 20)   // "themeToggleRequested"
    },
    "BaseWidget",
    "pageEvent",
    "",
    "PageEvent",
    "event",
    "data",
    "navigateRequested",
    "pageName",
    "systemFunctionRequested",
    "SystemFunction",
    "function",
    "logoutRequested",
    "deleteAccountRequested",
    "themeToggleRequested"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSBaseWidgetENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   68,    2, 0x06,    1 /* Public */,
       1,    1,   73,    2, 0x26,    4 /* Public | MethodCloned */,
       6,    2,   76,    2, 0x06,    6 /* Public */,
       6,    1,   81,    2, 0x26,    9 /* Public | MethodCloned */,
       8,    2,   84,    2, 0x06,   11 /* Public */,
       8,    1,   89,    2, 0x26,   14 /* Public | MethodCloned */,
      11,    0,   92,    2, 0x06,   16 /* Public */,
      12,    0,   93,    2, 0x06,   17 /* Public */,
      13,    0,   94,    2, 0x06,   18 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QVariant,    4,    5,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,    7,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, 0x80000000 | 9, QMetaType::QVariant,   10,    5,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject BaseWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSBaseWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSBaseWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSBaseWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BaseWidget, std::true_type>,
        // method 'pageEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<PageEvent, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariant &, std::false_type>,
        // method 'pageEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<PageEvent, std::false_type>,
        // method 'navigateRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariant &, std::false_type>,
        // method 'navigateRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'systemFunctionRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<SystemFunction, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariant &, std::false_type>,
        // method 'systemFunctionRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<SystemFunction, std::false_type>,
        // method 'logoutRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'deleteAccountRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'themeToggleRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void BaseWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BaseWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pageEvent((*reinterpret_cast< std::add_pointer_t<PageEvent>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2]))); break;
        case 1: _t->pageEvent((*reinterpret_cast< std::add_pointer_t<PageEvent>>(_a[1]))); break;
        case 2: _t->navigateRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2]))); break;
        case 3: _t->navigateRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->systemFunctionRequested((*reinterpret_cast< std::add_pointer_t<SystemFunction>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2]))); break;
        case 5: _t->systemFunctionRequested((*reinterpret_cast< std::add_pointer_t<SystemFunction>>(_a[1]))); break;
        case 6: _t->logoutRequested(); break;
        case 7: _t->deleteAccountRequested(); break;
        case 8: _t->themeToggleRequested(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BaseWidget::*)(PageEvent , const QVariant & );
            if (_t _q_method = &BaseWidget::pageEvent; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BaseWidget::*)(const QString & , const QVariant & );
            if (_t _q_method = &BaseWidget::navigateRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (BaseWidget::*)(SystemFunction , const QVariant & );
            if (_t _q_method = &BaseWidget::systemFunctionRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (BaseWidget::*)();
            if (_t _q_method = &BaseWidget::logoutRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (BaseWidget::*)();
            if (_t _q_method = &BaseWidget::deleteAccountRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (BaseWidget::*)();
            if (_t _q_method = &BaseWidget::themeToggleRequested; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
    }
}

const QMetaObject *BaseWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BaseWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSBaseWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int BaseWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void BaseWidget::pageEvent(PageEvent _t1, const QVariant & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 2
void BaseWidget::navigateRequested(const QString & _t1, const QVariant & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 4
void BaseWidget::systemFunctionRequested(SystemFunction _t1, const QVariant & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 6
void BaseWidget::logoutRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void BaseWidget::deleteAccountRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void BaseWidget::themeToggleRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
