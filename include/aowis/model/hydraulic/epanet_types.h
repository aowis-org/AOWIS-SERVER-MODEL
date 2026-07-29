#ifndef AOWIS_MODEL_HYDRAULIC_EPANET_TYPES_H
#define AOWIS_MODEL_HYDRAULIC_EPANET_TYPES_H

#include <optional>

#include <QString>

enum class EpanetNodeElevationInputType
{
    AbsoluteElevation = 0,
    BottomElevation = AbsoluteElevation,
    TotalElevation = AbsoluteElevation,
    TotalHead = AbsoluteElevation,
    TerrainElevationAndOffset = 1
};

using EpanetNodeTankElevationInputType = EpanetNodeElevationInputType;

enum class EpanetNodeTankGeometryInputType
{
    Cylindrical,
    UniformArea,
    VolumeAtMaximumLevel,
    VolumeCurve
};

enum class EpanetNodeQualitySourceType
{
    None,
    Concentration,
    MassBooster,
    FlowPacedBooster,
    SetpointBooster
};

enum class EpanetNodeTankMixingModel
{
    CompleteMix,
    TwoCompartment,
    FirstInFirstOut,
    LastInFirstOut
};

enum class EpanetNodeJunctionDemandSourceMethod
{
    ManualEstimation,
    MeterData,
    Scenario
};

enum class EpanetLinkPipeInitialStatus
{
    Open,
    Closed,
    CheckValve
};

enum class EpanetLinkPumpDefinitionType
{
    ConstantPower,
    OnePointCurve,
    ThreePointCurve,
    Library
};

enum class EpanetLinkPumpInitialStatus
{
    On,
    Off
};

enum class EpanetLinkPumpControlType
{
    None,
    LevelBased,
    TimeBased
};

enum class EpanetLinkPumpEfficiencyInputType
{
    Global,
    Constant,
    Curve
};

enum class EpanetLinkPumpEnergyPriceInputType
{
    Global,
    Constant,
    Pattern
};

enum class EpanetLinkValveType
{
    PRV,
    PSV,
    FCV,
    PBV,
    TCV,
    GPV,
    PCV
};

enum class EpanetLinkValveInitialStatus
{
    Active,
    Open,
    Closed
};

enum class EpanetOptionsHydraulicFlowUnits
{
    CubicFeetPerSecond,
    GallonsPerMinute,
    MillionGallonsPerDay,
    ImperialMillionGallonsPerDay,
    AcreFeetPerDay,
    LitersPerSecond,
    LitersPerMinute,
    MillionLitersPerDay,
    CubicMetersPerHour,
    CubicMetersPerDay,
    CubicMetersPerSecond
};

enum class EpanetOptionsHydraulicPressureUnits
{
    Psi,
    Kilopascals,
    Meters,
    Bar,
    Feet
};

enum class EpanetOptionsHydraulicHeadlossFormula
{
    HazenWilliams,
    DarcyWeisbach,
    ChezyManning
};

enum class EpanetOptionsHydraulicDemandModel
{
    DemandDriven,
    PressureDriven
};

enum class EpanetOptionsQualityAnalysis
{
    None,
    Chemical,
    WaterAge,
    SourceTrace
};

enum class EpanetOptionsReportStatistic
{
    Series,
    Average,
    Minimum,
    Maximum,
    Range
};

enum class EpanetOptionsReportStatus
{
    None,
    Normal,
    Full
};

enum class EpanetOptionsHydraulicUnbalancedAction
{
    Stop,
    Continue
};

enum class EpanetCurveType
{
    TankVolume,
    PumpHead,
    PumpEfficiency,
    ValveHeadloss,
    Generic,
    ValveCharacteristic
};

enum class EpanetControlSimpleType
{
    LowLevel,
    HighLevel,
    Timer,
    TimeOfDay
};

enum class EpanetControlActionType
{
    Open,
    Close,
    Setting
};

enum class EpanetControlRuleLogicalOperator
{
    If,
    And,
    Or
};

enum class EpanetControlRuleObject
{
    Node,
    Link,
    System
};

enum class EpanetControlRuleVariable
{
    Demand,
    Head,
    Grade,
    Level,
    Pressure,
    Flow,
    Status,
    Setting,
    Power,
    Time,
    ClockTime,
    FillTime,
    DrainTime
};

enum class EpanetControlRuleOperator
{
    Equal,
    NotEqual,
    LessOrEqual,
    GreaterOrEqual,
    Less,
    Greater,
    Is,
    IsNot,
    Below,
    Above
};

enum class EpanetControlRuleStatus
{
    Open,
    Closed,
    Active
};

enum class EpanetOptionsReportSelectionMode
{
    None,
    All,
    Selected
};

enum class EpanetMapBackdropUnits
{
    None,
    Feet,
    Meters,
    Degrees
};

enum class EpanetResultLinkPumpState
{
    CannotSupplyHead = 0,
    Closed = 2,
    Open = 3,
    CannotSupplyFlow = 5
};

enum class EpanetResultTimestepEventType
{
    ReportStep,
    HydraulicStep,
    QualityStep,
    TankEvent,
    ControlEvent
};

struct EpanetEntityMetadata
{
    QString comment;
    QString tag;
};

struct EpanetMapPosition
{
    double x = 0.0;
    double y = 0.0;
};

struct EpanetLinkVertex
{
    double latitude_deg = 0.0;
    double longitude_deg = 0.0;
    std::optional<EpanetMapPosition> map_position;
};

struct EpanetNodeQualitySource
{
    EpanetNodeQualitySourceType type = EpanetNodeQualitySourceType::None;
    double strength = 0.0;
    QString pattern_id;
};

#endif // AOWIS_MODEL_HYDRAULIC_EPANET_TYPES_H
