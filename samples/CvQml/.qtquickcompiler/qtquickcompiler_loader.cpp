/******************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Compiler.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#include <private/qv4function_p.h>
#include <private/qqmlirbuilder_p.h>
#include <private/qqmlglobal_p.h>
#include <private/qqmldirparser_p.h>
#include <QDir>
#include <QFileInfo>
#include <qqmlprivate.h>

namespace {

struct AOTCompilationUnit : public QV4::CompiledData::CompilationUnit
{
    virtual ~AOTCompilationUnit();
    virtual void linkBackendToEngine(QV4::ExecutionEngine *engine);

    virtual QV4::CompiledData::Unit *createUnitData(QmlIR::Document *irDocument);

    QV4::ReturnedValue (**functionTable)(QV4::ExecutionEngine*);
};

AOTCompilationUnit::~AOTCompilationUnit()
{
}

void AOTCompilationUnit::linkBackendToEngine(QV4::ExecutionEngine *engine)
{
    runtimeFunctions.resize(data->functionTableSize);
    runtimeFunctions.fill(0);
    for (int i = 0 ;i < runtimeFunctions.size(); ++i) {
        const QV4::CompiledData::Function *compiledFunction = data->functionAt(i);

        QV4::Function *runtimeFunction = new QV4::Function(engine, this, compiledFunction,
                                                           (QV4::ReturnedValue (*)(QV4::ExecutionEngine *, const uchar*)) functionTable[i]);
        runtimeFunctions[i] = runtimeFunction;
    }
}

QV4::CompiledData::Unit *AOTCompilationUnit::createUnitData(QmlIR::Document *irDocument)
{
    Q_ASSERT(irDocument->javaScriptCompilationUnit);
    QQmlRefPointer<QV4::CompiledData::CompilationUnit> compilationUnit = irDocument->javaScriptCompilationUnit;
    QV4::CompiledData::Unit *jsUnit = const_cast<QV4::CompiledData::Unit*>(irDocument->javaScriptCompilationUnit->data);

    QV4::Compiler::StringTableGenerator &stringTable = irDocument->jsGenerator.stringTable;

    // Collect signals that have had a change in signature (from onClicked to onClicked(mouse) for example)
    // and now need fixing in the QV4::CompiledData. Also register strings at the same time, to finalize
    // the string table.
    QVector<quint32> changedSignals;
    QVector<QQmlJS::AST::FormalParameterList*> changedSignalParameters;
    for (int i = 0, count = irDocument->objects.count(); i < count; ++i) {
        QmlIR::Object *o = irDocument->objects.at(i);
        for (QmlIR::Binding *binding = o->firstBinding(); binding; binding = binding->next) {
            if (!(binding->flags & QV4::CompiledData::Binding::IsSignalHandlerExpression))
                continue;

            quint32 functionIndex = binding->value.compiledScriptIndex;
            QmlIR::CompiledFunctionOrExpression *foe = o->functionsAndExpressions->slowAt(functionIndex);
            if (!foe)
                continue;

            // save absolute index
            changedSignals << o->runtimeFunctionIndices.at(functionIndex);

            Q_ASSERT(foe->node);
            QQmlJS::AST::FunctionDeclaration *decl = QQmlJS::AST::cast<QQmlJS::AST::FunctionDeclaration*>(foe->node);
            if (decl) {
                QQmlJS::AST::FormalParameterList *parameters = decl->formals;
                changedSignalParameters << parameters;

                for (; parameters; parameters = parameters->next)
                    stringTable.registerString(parameters->name.toString());
            }
        }
    }

    QVector<quint32> signalParameterNameTable;
    quint32 signalParameterNameTableOffset = jsUnit->unitSize;

    // Update signal signatures
    if (!changedSignals.isEmpty()) {
        if (jsUnit == compilationUnit->data) {
            char *unitCopy = (char*)malloc(jsUnit->unitSize);
            Q_CHECK_PTR(unitCopy);
            memcpy(unitCopy, jsUnit, jsUnit->unitSize);
            jsUnit = reinterpret_cast<QV4::CompiledData::Unit*>(unitCopy);
        }

        for (int i = 0; i < changedSignals.count(); ++i) {
            const uint functionIndex = changedSignals.at(i);
            // The data is now read-write due to the copy above, so the const_cast is ok.
            QV4::CompiledData::Function *function = const_cast<QV4::CompiledData::Function *>(jsUnit->functionAt(functionIndex));
            Q_ASSERT(function->nFormals == quint32(0));

            function->formalsOffset = signalParameterNameTableOffset - jsUnit->functionOffsetTable()[functionIndex];

            for (QQmlJS::AST::FormalParameterList *parameters = changedSignalParameters.at(i);
                 parameters; parameters = parameters->next) {
                signalParameterNameTable.append(stringTable.getStringId(parameters->name.toString()));
                function->nFormals = function->nFormals + 1;
            }

            // Hack to ensure an activation is created.
            function->flags |= QV4::CompiledData::Function::HasCatchOrWith | QV4::CompiledData::Function::HasDirectEval;

            signalParameterNameTableOffset += function->nFormals * sizeof(quint32);
        }
    }

    if (!signalParameterNameTable.isEmpty()) {
        Q_ASSERT(jsUnit != compilationUnit->data);
        const uint signalParameterTableSize = signalParameterNameTable.count() * sizeof(quint32);
        uint newSize = jsUnit->unitSize + signalParameterTableSize;
        const uint oldSize = jsUnit->unitSize;
        char *unitWithSignalParameters = (char*)realloc(jsUnit, newSize);
        Q_CHECK_PTR(unitWithSignalParameters);
        memcpy(unitWithSignalParameters + oldSize, signalParameterNameTable.constData(), signalParameterTableSize);
        jsUnit = reinterpret_cast<QV4::CompiledData::Unit*>(unitWithSignalParameters);
        jsUnit->unitSize = newSize;
    }

    if (jsUnit != compilationUnit->data)
        jsUnit->flags &= ~QV4::CompiledData::Unit::StaticData;

    return jsUnit;
}

struct Registry {
    Registry();

    QMutex mutex;

    QHash<QString, const QQmlPrivate::CachedQmlUnit*> resourcePathToCachedUnit;

    static const QQmlPrivate::CachedQmlUnit *lookupCachedUnit(const QUrl &url);
};

Q_GLOBAL_STATIC(Registry, registry)

Registry::Registry()
{
    QQmlPrivate::RegisterQmlUnitCacheHook registration;
    registration.version = 0;
    registration.lookupCachedQmlUnit = &lookupCachedUnit;
    QQmlPrivate::qmlregister(QQmlPrivate::QmlUnitCacheHookRegistration, &registration);
}

const QQmlPrivate::CachedQmlUnit *Registry::lookupCachedUnit(const QUrl &url)
{
    QString resourcePath;
    if (url.scheme() == QLatin1String("qrc"))
        resourcePath = url.path();

    resourcePath = QDir::cleanPath(resourcePath);

    if (resourcePath.isEmpty())
        return 0;

    if (!resourcePath.startsWith(QLatin1Char('/')))
        resourcePath.prepend(QLatin1Char('/'));

    Registry *r = registry();
    QMutexLocker locker(&r->mutex);
    return r->resourcePathToCachedUnit.value(resourcePath, 0);
}

QV4::CompiledData::CompilationUnit *createCompilationUnit(const QV4::CompiledData::Unit *unitData, QV4::ReturnedValue (**functions)(QV4::ExecutionEngine *))
{
    AOTCompilationUnit *unit = new AOTCompilationUnit;
    unit->data = const_cast<QV4::CompiledData::Unit*>(unitData);
    unit->functionTable = functions;
    return unit;
}
} // anonymous namespace
#include <qqmlprivate.h>

static const unsigned char qt_resource_tree[] = {
0,
0,0,0,0,2,0,0,0,1,0,0,0,1,0,0,0,
8,0,0,0,0,0,1,0,0,0,0};
static const unsigned char qt_resource_names[] = {
0,
1,0,0,0,47,0,47,0,8,8,1,90,92,0,109,0,
97,0,105,0,110,0,46,0,113,0,109,0,108};
static const unsigned char qt_resource_empty_payout[] = { 0, 0, 0, 0, 0 };
QT_BEGIN_NAMESPACE
extern Q_CORE_EXPORT bool qRegisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);
QT_END_NAMESPACE
typedef QV4::ReturnedValue (*AOTFunction)(QV4::ExecutionEngine*);
namespace QtQuickCompilerGeneratedModule
{
    namespace __main_qml
    {
        extern const unsigned char qmlData[];
        extern AOTFunction moduleFunctions[];
        QV4::CompiledData::CompilationUnit *createCompilationUnit() { return ::createCompilationUnit(reinterpret_cast<const QV4::CompiledData::Unit*>(qmlData), moduleFunctions); }
        const QQmlPrivate::CachedQmlUnit unit = {
            reinterpret_cast<const QV4::CompiledData::Unit*>(qmlData), &createCompilationUnit, nullptr
        };

}
}
static void registerCompilationUnits(){
    static bool initialized = false;
    if (initialized) return;
    initialized = true;
    ::Registry *r = registry();
    QMutexLocker locker(&r->mutex);
    r->resourcePathToCachedUnit.insert(QStringLiteral("/main.qml"), &QtQuickCompilerGeneratedModule::__main_qml::unit);
QT_PREPEND_NAMESPACE(qRegisterResourceData)(/*version*/0x01, qt_resource_tree, qt_resource_names, qt_resource_empty_payout);

}
Q_CONSTRUCTOR_FUNCTION(registerCompilationUnits);
int QT_MANGLE_NAMESPACE(qInitResources_qml)() {
    ::registerCompilationUnits();
    Q_INIT_RESOURCE(qml_qtquickcompiler);
    return 1;
}
