/****************************************************************************
** Meta object code from reading C++ file 'workstation_bridge.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/workstation_bridge.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'workstation_bridge.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
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
struct qt_meta_tag_ZN17WorkstationBridgeE_t {};
} // unnamed namespace

template <> constexpr inline auto WorkstationBridge::qt_create_metaobjectdata<qt_meta_tag_ZN17WorkstationBridgeE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "WorkstationBridge",
        "workingDirectoryChanged",
        "",
        "gitInfoChanged",
        "busyChanged",
        "databaseOpenChanged",
        "useSystemTerminalChanged",
        "terminalCleared",
        "outputLine",
        "text",
        "isStdErr",
        "onRunnerOutput",
        "onRunnerFinished",
        "exitCode",
        "initDatabase",
        "presetGroups",
        "QVariantList",
        "recentRuns",
        "limit",
        "runPreset",
        "presetId",
        "runShellCommand",
        "command",
        "stopCommand",
        "refreshGit",
        "gitPull",
        "gitPush",
        "gitCommitAll",
        "message",
        "gitDiscardChanges",
        "pickFolderUrl",
        "QUrl",
        "url",
        "urlToLocalPath",
        "saveCustomPreset",
        "name",
        "cwd",
        "buttonColor",
        "deleteCustomPreset",
        "id",
        "updateCustomPreset",
        "workingDirectory",
        "gitBranch",
        "gitStatus",
        "gitRemote",
        "gitRepo",
        "gitDirty",
        "busy",
        "databaseOpen",
        "databasePath",
        "useSystemTerminal"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'workingDirectoryChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'gitInfoChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'busyChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'databaseOpenChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'useSystemTerminalChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'terminalCleared'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'outputLine'
        QtMocHelpers::SignalData<void(const QString &, bool)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 }, { QMetaType::Bool, 10 },
        }}),
        // Slot 'onRunnerOutput'
        QtMocHelpers::SlotData<void(const QString &, bool)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 9 }, { QMetaType::Bool, 10 },
        }}),
        // Slot 'onRunnerFinished'
        QtMocHelpers::SlotData<void(int)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Method 'initDatabase'
        QtMocHelpers::MethodData<bool()>(14, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'presetGroups'
        QtMocHelpers::MethodData<QVariantList()>(15, 2, QMC::AccessPublic, 0x80000000 | 16),
        // Method 'recentRuns'
        QtMocHelpers::MethodData<QVariantList(int)>(17, 2, QMC::AccessPublic, 0x80000000 | 16, {{
            { QMetaType::Int, 18 },
        }}),
        // Method 'recentRuns'
        QtMocHelpers::MethodData<QVariantList()>(17, 2, QMC::AccessPublic | QMC::MethodCloned, 0x80000000 | 16),
        // Method 'runPreset'
        QtMocHelpers::MethodData<void(int)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 20 },
        }}),
        // Method 'runShellCommand'
        QtMocHelpers::MethodData<void(const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 22 },
        }}),
        // Method 'stopCommand'
        QtMocHelpers::MethodData<void()>(23, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'refreshGit'
        QtMocHelpers::MethodData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'gitPull'
        QtMocHelpers::MethodData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'gitPush'
        QtMocHelpers::MethodData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'gitCommitAll'
        QtMocHelpers::MethodData<void(const QString &)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 28 },
        }}),
        // Method 'gitDiscardChanges'
        QtMocHelpers::MethodData<void()>(29, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'pickFolderUrl'
        QtMocHelpers::MethodData<QString(const QUrl &)>(30, 2, QMC::AccessPublic, QMetaType::QString, {{
            { 0x80000000 | 31, 32 },
        }}),
        // Method 'urlToLocalPath'
        QtMocHelpers::MethodData<QString(const QUrl &) const>(33, 2, QMC::AccessPublic, QMetaType::QString, {{
            { 0x80000000 | 31, 32 },
        }}),
        // Method 'saveCustomPreset'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(34, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 35 }, { QMetaType::QString, 22 }, { QMetaType::QString, 36 }, { QMetaType::QString, 37 },
        }}),
        // Method 'saveCustomPreset'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &)>(34, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Bool, {{
            { QMetaType::QString, 35 }, { QMetaType::QString, 22 }, { QMetaType::QString, 36 },
        }}),
        // Method 'deleteCustomPreset'
        QtMocHelpers::MethodData<bool(int)>(38, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 39 },
        }}),
        // Method 'updateCustomPreset'
        QtMocHelpers::MethodData<bool(int, const QString &, const QString &, const QString &, const QString &)>(40, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 39 }, { QMetaType::QString, 35 }, { QMetaType::QString, 22 }, { QMetaType::QString, 36 },
            { QMetaType::QString, 37 },
        }}),
        // Method 'updateCustomPreset'
        QtMocHelpers::MethodData<bool(int, const QString &, const QString &, const QString &)>(40, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Bool, {{
            { QMetaType::Int, 39 }, { QMetaType::QString, 35 }, { QMetaType::QString, 22 }, { QMetaType::QString, 36 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'workingDirectory'
        QtMocHelpers::PropertyData<QString>(41, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'gitBranch'
        QtMocHelpers::PropertyData<QString>(42, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'gitStatus'
        QtMocHelpers::PropertyData<QString>(43, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'gitRemote'
        QtMocHelpers::PropertyData<QString>(44, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'gitRepo'
        QtMocHelpers::PropertyData<bool>(45, QMetaType::Bool, QMC::DefaultPropertyFlags, 1),
        // property 'gitDirty'
        QtMocHelpers::PropertyData<bool>(46, QMetaType::Bool, QMC::DefaultPropertyFlags, 1),
        // property 'busy'
        QtMocHelpers::PropertyData<bool>(47, QMetaType::Bool, QMC::DefaultPropertyFlags, 2),
        // property 'databaseOpen'
        QtMocHelpers::PropertyData<bool>(48, QMetaType::Bool, QMC::DefaultPropertyFlags, 3),
        // property 'databasePath'
        QtMocHelpers::PropertyData<QString>(49, QMetaType::QString, QMC::DefaultPropertyFlags, 3),
        // property 'useSystemTerminal'
        QtMocHelpers::PropertyData<bool>(50, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WorkstationBridge, qt_meta_tag_ZN17WorkstationBridgeE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject WorkstationBridge::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17WorkstationBridgeE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17WorkstationBridgeE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17WorkstationBridgeE_t>.metaTypes,
    nullptr
} };

void WorkstationBridge::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WorkstationBridge *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->workingDirectoryChanged(); break;
        case 1: _t->gitInfoChanged(); break;
        case 2: _t->busyChanged(); break;
        case 3: _t->databaseOpenChanged(); break;
        case 4: _t->useSystemTerminalChanged(); break;
        case 5: _t->terminalCleared(); break;
        case 6: _t->outputLine((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 7: _t->onRunnerOutput((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 8: _t->onRunnerFinished((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 9: { bool _r = _t->initDatabase();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 10: { QVariantList _r = _t->presetGroups();
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 11: { QVariantList _r = _t->recentRuns((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 12: { QVariantList _r = _t->recentRuns();
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 13: _t->runPreset((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 14: _t->runShellCommand((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->stopCommand(); break;
        case 16: _t->refreshGit(); break;
        case 17: _t->gitPull(); break;
        case 18: _t->gitPush(); break;
        case 19: _t->gitCommitAll((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->gitDiscardChanges(); break;
        case 21: { QString _r = _t->pickFolderUrl((*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 22: { QString _r = _t->urlToLocalPath((*reinterpret_cast<std::add_pointer_t<QUrl>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 23: { bool _r = _t->saveCustomPreset((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 24: { bool _r = _t->saveCustomPreset((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 25: { bool _r = _t->deleteCustomPreset((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 26: { bool _r = _t->updateCustomPreset((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[5])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 27: { bool _r = _t->updateCustomPreset((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WorkstationBridge::*)()>(_a, &WorkstationBridge::workingDirectoryChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkstationBridge::*)()>(_a, &WorkstationBridge::gitInfoChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkstationBridge::*)()>(_a, &WorkstationBridge::busyChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkstationBridge::*)()>(_a, &WorkstationBridge::databaseOpenChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkstationBridge::*)()>(_a, &WorkstationBridge::useSystemTerminalChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkstationBridge::*)()>(_a, &WorkstationBridge::terminalCleared, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (WorkstationBridge::*)(const QString & , bool )>(_a, &WorkstationBridge::outputLine, 6))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->workingDirectory(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->gitBranch(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->gitStatus(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->gitRemote(); break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->gitRepo(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->gitDirty(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->busy(); break;
        case 7: *reinterpret_cast<bool*>(_v) = _t->databaseOpen(); break;
        case 8: *reinterpret_cast<QString*>(_v) = _t->databasePath(); break;
        case 9: *reinterpret_cast<bool*>(_v) = _t->useSystemTerminal(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setWorkingDirectory(*reinterpret_cast<QString*>(_v)); break;
        case 9: _t->setUseSystemTerminal(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *WorkstationBridge::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorkstationBridge::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17WorkstationBridgeE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WorkstationBridge::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 28)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 28;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void WorkstationBridge::workingDirectoryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void WorkstationBridge::gitInfoChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void WorkstationBridge::busyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void WorkstationBridge::databaseOpenChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void WorkstationBridge::useSystemTerminalChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void WorkstationBridge::terminalCleared()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void WorkstationBridge::outputLine(const QString & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}
QT_WARNING_POP
