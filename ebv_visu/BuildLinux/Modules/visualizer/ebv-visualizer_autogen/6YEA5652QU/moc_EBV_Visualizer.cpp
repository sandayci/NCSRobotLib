/****************************************************************************
** Meta object code from reading C++ file 'EBV_Visualizer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../Modules/visualizer/include/EBV_Visualizer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EBV_Visualizer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VisualizerPlugin_t {
    QByteArrayData data[1];
    char stringdata0[17];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VisualizerPlugin_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VisualizerPlugin_t qt_meta_stringdata_VisualizerPlugin = {
    {
QT_MOC_LITERAL(0, 0, 16) // "VisualizerPlugin"

    },
    "VisualizerPlugin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VisualizerPlugin[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void VisualizerPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject VisualizerPlugin::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_VisualizerPlugin.data,
      qt_meta_data_VisualizerPlugin,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *VisualizerPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VisualizerPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_VisualizerPlugin.stringdata0))
        return static_cast<void*>(const_cast< VisualizerPlugin*>(this));
    return QWidget::qt_metacast(_clname);
}

int VisualizerPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_VisualizerWindow_t {
    QByteArrayData data[7];
    char stringdata0[99];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VisualizerWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VisualizerWindow_t qt_meta_stringdata_VisualizerWindow = {
    {
QT_MOC_LITERAL(0, 0, 16), // "VisualizerWindow"
QT_MOC_LITERAL(1, 17, 20), // "readPendingDatagrams"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 19), // "changeAddressServer"
QT_MOC_LITERAL(4, 59, 16), // "changeDockLayout"
QT_MOC_LITERAL(5, 76, 18), // "Qt::DockWidgetArea"
QT_MOC_LITERAL(6, 95, 3) // "dir"

    },
    "VisualizerWindow\0readPendingDatagrams\0"
    "\0changeAddressServer\0changeDockLayout\0"
    "Qt::DockWidgetArea\0dir"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VisualizerWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x08 /* Private */,
       3,    0,   30,    2, 0x08 /* Private */,
       4,    1,   31,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,

       0        // eod
};

void VisualizerWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        VisualizerWindow *_t = static_cast<VisualizerWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->readPendingDatagrams(); break;
        case 1: _t->changeAddressServer(); break;
        case 2: _t->changeDockLayout((*reinterpret_cast< Qt::DockWidgetArea(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject VisualizerWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_VisualizerWindow.data,
      qt_meta_data_VisualizerWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *VisualizerWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VisualizerWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_VisualizerWindow.stringdata0))
        return static_cast<void*>(const_cast< VisualizerWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int VisualizerWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
struct qt_meta_stringdata_VisualizerGL_t {
    QByteArrayData data[17];
    char stringdata0[250];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VisualizerGL_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VisualizerGL_t qt_meta_stringdata_VisualizerGL = {
    {
QT_MOC_LITERAL(0, 0, 12), // "VisualizerGL"
QT_MOC_LITERAL(1, 13, 16), // "xRotationChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 16), // "yRotationChanged"
QT_MOC_LITERAL(4, 48, 16), // "zRotationChanged"
QT_MOC_LITERAL(5, 65, 19), // "xTranslationChanged"
QT_MOC_LITERAL(6, 85, 19), // "yTranslationChanged"
QT_MOC_LITERAL(7, 105, 19), // "zTranslationChanged"
QT_MOC_LITERAL(8, 125, 16), // "toggleViewSkybox"
QT_MOC_LITERAL(9, 142, 1), // "t"
QT_MOC_LITERAL(10, 144, 15), // "toggleViewFloor"
QT_MOC_LITERAL(11, 160, 23), // "setCameraRotSensitivity"
QT_MOC_LITERAL(12, 184, 1), // "s"
QT_MOC_LITERAL(13, 186, 25), // "setCameraTransSensitivity"
QT_MOC_LITERAL(14, 212, 10), // "updateView"
QT_MOC_LITERAL(15, 223, 13), // "updateViewDir"
QT_MOC_LITERAL(16, 237, 12) // "keyMoveEvent"

    },
    "VisualizerGL\0xRotationChanged\0\0"
    "yRotationChanged\0zRotationChanged\0"
    "xTranslationChanged\0yTranslationChanged\0"
    "zTranslationChanged\0toggleViewSkybox\0"
    "t\0toggleViewFloor\0setCameraRotSensitivity\0"
    "s\0setCameraTransSensitivity\0updateView\0"
    "updateViewDir\0keyMoveEvent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VisualizerGL[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x06 /* Public */,
       3,    0,   80,    2, 0x06 /* Public */,
       4,    0,   81,    2, 0x06 /* Public */,
       5,    0,   82,    2, 0x06 /* Public */,
       6,    0,   83,    2, 0x06 /* Public */,
       7,    0,   84,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   85,    2, 0x0a /* Public */,
      10,    1,   88,    2, 0x0a /* Public */,
      11,    1,   91,    2, 0x0a /* Public */,
      13,    1,   94,    2, 0x0a /* Public */,
      14,    0,   97,    2, 0x08 /* Private */,
      15,    0,   98,    2, 0x08 /* Private */,
      16,    0,   99,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void VisualizerGL::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        VisualizerGL *_t = static_cast<VisualizerGL *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->xRotationChanged(); break;
        case 1: _t->yRotationChanged(); break;
        case 2: _t->zRotationChanged(); break;
        case 3: _t->xTranslationChanged(); break;
        case 4: _t->yTranslationChanged(); break;
        case 5: _t->zTranslationChanged(); break;
        case 6: _t->toggleViewSkybox((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->toggleViewFloor((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->setCameraRotSensitivity((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->setCameraTransSensitivity((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->updateView(); break;
        case 11: _t->updateViewDir(); break;
        case 12: _t->keyMoveEvent(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (VisualizerGL::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VisualizerGL::xRotationChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (VisualizerGL::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VisualizerGL::yRotationChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (VisualizerGL::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VisualizerGL::zRotationChanged)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (VisualizerGL::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VisualizerGL::xTranslationChanged)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (VisualizerGL::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VisualizerGL::yTranslationChanged)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (VisualizerGL::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VisualizerGL::zTranslationChanged)) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject VisualizerGL::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_VisualizerGL.data,
      qt_meta_data_VisualizerGL,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *VisualizerGL::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VisualizerGL::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_VisualizerGL.stringdata0))
        return static_cast<void*>(const_cast< VisualizerGL*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int VisualizerGL::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void VisualizerGL::xRotationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void VisualizerGL::yRotationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void VisualizerGL::zRotationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void VisualizerGL::xTranslationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void VisualizerGL::yTranslationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void VisualizerGL::zTranslationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
