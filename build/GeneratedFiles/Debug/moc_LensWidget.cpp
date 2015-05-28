/****************************************************************************
** Meta object code from reading C++ file 'LensWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/Lens/LensWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LensWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LensWidget_t {
    QByteArrayData data[23];
    char stringdata[246];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LensWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LensWidget_t qt_meta_stringdata_LensWidget = {
    {
QT_MOC_LITERAL(0, 0, 10), // "LensWidget"
QT_MOC_LITERAL(1, 11, 12), // "tableUpdated"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 4), // "load"
QT_MOC_LITERAL(4, 30, 8), // "filename"
QT_MOC_LITERAL(5, 39, 11), // "cellChanged"
QT_MOC_LITERAL(6, 51, 1), // "i"
QT_MOC_LITERAL(7, 53, 11), // "cellClicked"
QT_MOC_LITERAL(8, 65, 23), // "horizontalHeaderClicked"
QT_MOC_LITERAL(9, 89, 6), // "column"
QT_MOC_LITERAL(10, 96, 21), // "verticalHeaderClicked"
QT_MOC_LITERAL(11, 118, 3), // "row"
QT_MOC_LITERAL(12, 122, 20), // "zoomDeviationChanged"
QT_MOC_LITERAL(13, 143, 5), // "value"
QT_MOC_LITERAL(14, 149, 21), // "focusDeviationChanged"
QT_MOC_LITERAL(15, 171, 6), // "update"
QT_MOC_LITERAL(16, 178, 17), // "onLensDataChanged"
QT_MOC_LITERAL(17, 196, 4), // "zoom"
QT_MOC_LITERAL(18, 201, 5), // "focus"
QT_MOC_LITERAL(19, 207, 4), // "iris"
QT_MOC_LITERAL(20, 212, 3), // "fov"
QT_MOC_LITERAL(21, 216, 22), // "onEncodeFeedingToggled"
QT_MOC_LITERAL(22, 239, 6) // "toggle"

    },
    "LensWidget\0tableUpdated\0\0load\0filename\0"
    "cellChanged\0i\0cellClicked\0"
    "horizontalHeaderClicked\0column\0"
    "verticalHeaderClicked\0row\0"
    "zoomDeviationChanged\0value\0"
    "focusDeviationChanged\0update\0"
    "onLensDataChanged\0zoom\0focus\0iris\0fov\0"
    "onEncodeFeedingToggled\0toggle"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LensWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   70,    2, 0x0a /* Public */,
       5,    2,   73,    2, 0x0a /* Public */,
       7,    2,   78,    2, 0x0a /* Public */,
       8,    1,   83,    2, 0x0a /* Public */,
      10,    1,   86,    2, 0x0a /* Public */,
      12,    1,   89,    2, 0x0a /* Public */,
      14,    1,   92,    2, 0x0a /* Public */,
      15,    0,   95,    2, 0x0a /* Public */,
      16,    4,   96,    2, 0x0a /* Public */,
      21,    1,  105,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Bool, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    6,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    6,    2,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Double,   17,   18,   19,   20,
    QMetaType::Void, QMetaType::Bool,   22,

       0        // eod
};

void LensWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LensWidget *_t = static_cast<LensWidget *>(_o);
        switch (_id) {
        case 0: _t->tableUpdated(); break;
        case 1: { bool _r = _t->load((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: _t->cellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->cellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->horizontalHeaderClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->verticalHeaderClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->zoomDeviationChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->focusDeviationChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->update(); break;
        case 9: _t->onLensDataChanged((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 10: _t->onEncodeFeedingToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LensWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LensWidget::tableUpdated)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject LensWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_LensWidget.data,
      qt_meta_data_LensWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LensWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LensWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LensWidget.stringdata))
        return static_cast<void*>(const_cast< LensWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int LensWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void LensWidget::tableUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
