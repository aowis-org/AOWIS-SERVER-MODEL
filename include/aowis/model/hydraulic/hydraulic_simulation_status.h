#ifndef AOWIS_MODEL_HYDRAULIC_SIMULATION_STATUS_H
#define AOWIS_MODEL_HYDRAULIC_SIMULATION_STATUS_H

#include <QMetaType>
#include <QObject>
#include <QString>
#include <QStringList>

class HydraulicSimulationStatusEnums
{
    Q_GADGET

public:
    enum class Stage
    {
        None,

        CreateBackendContext,
        InitializeSimulation,
        ConfigureOptions,

        BuildNetwork,
        AddPattern,
        AddCurve,
        AddReservoir,
        AddJunction,
        AddTank,
        AddPipe,
        AddPump,
        AddValve,
        AddControl,
        AddRule,

        RunHydraulics,
        RunQuality,

        ReadResults,
        ReadJunctionResults,
        ReadReservoirResults,
        ReadTankResults,
        ReadPipeResults,
        ReadPumpResults,
        ReadValveResults,
        ReadStatistics,
        ReadEnergyUsage,

        CloseHydraulics,
        CloseQuality,
        SaveHydraulics,
        GenerateReport,
        Cleanup
    };
    Q_ENUM(Stage)

    enum class Operation
    {
        None,

        CreateBackendContext,
        DestroyBackendContext,
        Initialize,

        SetMetadata,
        ConfigureTime,
        ConfigureHydraulics,
        ConfigureQuality,
        ConfigureReport,

        AddPattern,
        AddCurve,
        AddNode,
        AddDemand,
        AddLink,
        AddControl,
        AddRule,

        SetEntityMetadata,
        SetEntityGeometry,

        OpenHydraulics,
        InitializeHydraulics,
        RunHydraulics,
        AdvanceHydraulics,
        CloseHydraulics,
        SaveHydraulics,

        OpenQuality,
        InitializeQuality,
        RunQuality,
        AdvanceQuality,
        StepQuality,
        CloseQuality,

        GenerateReport,

        ResolveEntity,
        ReadNodeResult,
        ReadLinkResult,
        ReadStatistic,
        ReadBackendError
    };
    Q_ENUM(Operation)

    enum class Property
    {
        None,

        Elevation,
        Demand,
        DemandDeficit,
        Emitter,
        Leakage,

        Head,
        Pressure,
        Level,
        Volume,

        Flow,
        Velocity,
        Headloss,

        Quality,
        SourceMass,

        Status,
        Setting,
        Energy,
        Efficiency,

        InitialStatus
    };
    Q_ENUM(Property)

    enum class EntityType
    {
        None,

        Project,
        Network,

        Node,
        Junction,
        Reservoir,
        Tank,

        Link,
        Pipe,
        Pump,
        Valve,

        Pattern,
        Curve,
        Control,
        Rule,

        HydraulicSolver,
        QualitySolver,
        Report,
        Result
    };
    Q_ENUM(EntityType)
};

using HydraulicSimulationStatusStage = HydraulicSimulationStatusEnums::Stage;
using HydraulicSimulationStatusOperation = HydraulicSimulationStatusEnums::Operation;
using HydraulicSimulationStatusProperty = HydraulicSimulationStatusEnums::Property;
using HydraulicSimulationStatusEntityType = HydraulicSimulationStatusEnums::EntityType;

struct HydraulicSimulationStatusEntity
{
    HydraulicSimulationStatusEntityType type = HydraulicSimulationStatusEntityType::None;

    QString id;
    int index = 0;
};

struct HydraulicSimulationStatus
{
    bool success = true;

    HydraulicSimulationStatusStage stage = HydraulicSimulationStatusStage::None;
    HydraulicSimulationStatusOperation operation = HydraulicSimulationStatusOperation::None;
    HydraulicSimulationStatusProperty property = HydraulicSimulationStatusProperty::None;

    HydraulicSimulationStatusEntity entity;

    QString message;
    QStringList details;

    // Optional diagnostics supplied by the active simulation backend.
    QString backend_name;
    int backend_error_code = 0;
    QString backend_operation;
    QString message_backend;
};

Q_DECLARE_METATYPE(HydraulicSimulationStatus)

#endif // AOWIS_MODEL_HYDRAULIC_SIMULATION_STATUS_H
