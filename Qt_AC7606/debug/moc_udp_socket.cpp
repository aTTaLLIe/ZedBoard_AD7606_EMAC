/****************************************************************************
** Meta object code from reading C++ file 'udp_socket.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../udp_socket.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'udp_socket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_udp_socket_t {
    QByteArrayData data[5];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_udp_socket_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_udp_socket_t qt_meta_stringdata_udp_socket = {
    {
QT_MOC_LITERAL(0, 0, 10), // "udp_socket"
QT_MOC_LITERAL(1, 11, 10), // "Ready_Read"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 17), // "Send_Result_Array"
QT_MOC_LITERAL(4, 41, 11) // "QByteArray*"

    },
    "udp_socket\0Ready_Read\0\0Send_Result_Array\0"
    "QByteArray*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_udp_socket[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,
       3,    1,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    2,

       0        // eod
};

void udp_socket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        udp_socket *_t = static_cast<udp_socket *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Ready_Read(); break;
        case 1: _t->Send_Result_Array((*reinterpret_cast< QByteArray*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (udp_socket::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&udp_socket::Ready_Read)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (udp_socket::*)(QByteArray * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&udp_socket::Send_Result_Array)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject udp_socket::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_udp_socket.data,
      qt_meta_data_udp_socket,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *udp_socket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *udp_socket::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_udp_socket.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int udp_socket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void udp_socket::Ready_Read()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void udp_socket::Send_Result_Array(QByteArray * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
