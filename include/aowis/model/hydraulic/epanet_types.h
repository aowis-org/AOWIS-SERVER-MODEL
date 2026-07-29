#ifndef AOWIS_MODEL_HYDRAULIC_EPANET_TYPES_H
#define AOWIS_MODEL_HYDRAULIC_EPANET_TYPES_H

#include <optional>
#include <QString>
#include <QUuid>

enum class ElevationInputType
{
    AbsoluteElevation = 0,
    BottomElevation = AbsoluteElevation,
    TotalElevation = AbsoluteElevation,
    TotalHead = AbsoluteElevation,
    TerrainElevationAndOffset = 1
};

using TankElevationInputType = ElevationInputType;

enum class TankGeometryInputType
{
    Cylindrical,
    UniformArea,
    VolumeAtMaximumLevel,
    VolumeCurve
};

enum class QualitySourceType
{
    None,
    Concentration,
    MassBooster,
    FlowPacedBooster,
    SetpointBooster
};

enum class TankMixingModel
{
    CompleteMix,
    TwoCompartment,
    FirstInFirstOut,
    LastInFirstOut
};

enum class DemandSourceMethod
{
    ManualEstimation,
    MeterData,
    Scenario
};

enum class PipeInitialStatus
{
    Open,
    Closed,
    CheckValve
};

enum class PumpDefinitionType
{
    ConstantPower,
    OnePointCurve,
    ThreePointCurve,
    Library
};

enum class PumpInitialStatus
{
    On,
    Off
};

enum class PumpControlType
{
    None,
    LevelBased,
    TimeBased
};

enum class PumpEfficiencyInputType
{
    Global,
    Constant,
    Curve
};

enum class PumpEnergyPriceInputType
{
    Global,
    Constant,
    Pattern
};

enum class ValveType
{
    PRV,
    PSV,
    FCV,
    PBV,
    TCV,
    GPV,
    PCV
};

enum class ValveInitialStatus
{
    Active,
    Open,
    Closed
};

enum class EpanetFlowUnits
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

enum class EpanetPressureUnits
{
    Psi,
    Kilopascals,
    Meters,
    Bar,
    Feet
};

enum class EpanetHeadlossFormula
{
    HazenWilliams,
    DarcyWeisbach,
    ChezyManning
};

enum class EpanetDemandModel
{
    DemandDriven,
    PressureDriven
};

enum class EpanetQualityAnalysis
{
    None,
    Chemical,
    WaterAge,
    SourceTrace
};

enum class EpanetReportingStatistic
{
    Series,
    Average,
    Minimum,
    Maximum,
    Range
};

enum class EpanetStatusReport
{
    None,
    Normal,
    Full
};

enum class EpanetUnbalancedAction
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

enum class EpanetSimpleControlType
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

enum class EpanetRuleLogicalOperator
{
    If,
    And,
    Or
};

enum class EpanetRuleObject
{
    Node,
    Link,
    System
};

enum class EpanetRuleVariable
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

enum class EpanetRuleOperator
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

enum class EpanetRuleStatus
{
    Open,
    Closed,
    Active
};

enum class EpanetReportSelection
{
    None,
    All,
    Selected
};

enum class EpanetBackdropUnits
{
    None,
    Feet,
    Meters,
    Degrees
};

enum class EpanetPumpState
{
    CannotSupplyHead = 0,
    Closed = 2,
    Open = 3,
    CannotSupplyFlow = 5
};

enum class EpanetTimestepEventType
{
    ReportStep,
    HydraulicStep,
    QualityStep,
    TankEvent,
    ControlEvent
};

struct EpanetObjectMetadata
{
    QString comment;
    QString tag;
};

struct EpanetMapPoint
{
    double x = 0.0;
    double y = 0.0;
};

struct EpanetLinkVertex
{
    double latitude_deg = 0.0;
    double longitude_deg = 0.0;
    std::optional<EpanetMapPoint> epanet_position;
};

struct EpanetQualitySource
{
    QualitySourceType type = QualitySourceType::None;
    double strength = 0.0;
    QString pattern_id;
};

#endif // AOWIS_MODEL_HYDRAULIC_EPANET_TYPES_H
