#ifndef AOWIS_MODEL_HYDRAULIC_TYPES_H
#define AOWIS_MODEL_HYDRAULIC_TYPES_H

#include <optional>

#include <QDate>
#include <QString>
#include <QUuid>

#include "../entity.h"
#include "../gis.h"

enum class HydraulicNodeElevationInputType
{
    AbsoluteElevation = 0,
    BottomElevation = AbsoluteElevation,
    TotalElevation = AbsoluteElevation,
    TotalHead = AbsoluteElevation,
    TerrainElevationAndOffset = 1
};

using HydraulicNodeTankElevationInputType = HydraulicNodeElevationInputType;

enum class HydraulicNodeTankGeometryInputType
{
    Cylindrical,
    UniformArea,
    VolumeAtMaximumLevel,
    VolumeCurve
};

enum class HydraulicNodeQualitySourceType
{
    None,
    Concentration,
    MassBooster,
    FlowPacedBooster,
    SetpointBooster
};

enum class HydraulicNodeTankMixingModel
{
    CompleteMix,
    TwoCompartment,
    FirstInFirstOut,
    LastInFirstOut
};

enum class HydraulicTimePatternMode
{
    Constant,
    TimePattern
};

enum class HydraulicNodeJunctionDemandSourceMethod
{
    ManualEstimation,
    MeterData,
    Scenario
};

enum class HydraulicLinkPipeInitialStatus
{
    Open,
    Closed,
    CheckValve
};

enum class HydraulicLinkPumpDefinitionType
{
    ConstantPower,
    OnePointCurve,
    ThreePointCurve,
    Library,
    MultiPointCurve
};

enum class HydraulicLinkPumpInitialStatus
{
    On,
    Off
};

enum class HydraulicLinkPumpControlType
{
    None,
    LevelBased,
    TimeBased
};

enum class HydraulicLinkPumpEfficiencyInputType
{
    Global,
    Constant,
    Curve
};

enum class HydraulicLinkPumpEnergyPriceInputType
{
    Global,
    Constant,
    Pattern
};

enum class HydraulicLinkValveType
{
    PRV,
    PSV,
    FCV,
    PBV,
    TCV,
    GPV,
    PCV
};

enum class HydraulicLinkValveInitialStatus
{
    Active,
    Open,
    Closed
};

enum class HydraulicFlowUnit
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

enum class HydraulicPressureUnit
{
    Psi,
    Kilopascals,
    Meters,
    Bar,
    Feet
};

enum class HydraulicHeadlossFormula
{
    HazenWilliams,
    DarcyWeisbach,
    ChezyManning
};

enum class HydraulicDemandModel
{
    DemandDriven,
    PressureDriven
};

enum class WaterQualityAnalysisType
{
    None,
    Chemical,
    WaterAge,
    SourceTrace
};

enum class HydraulicSimulationReportStatistic
{
    Series,
    Average,
    Minimum,
    Maximum,
    Range
};

enum class HydraulicSimulationReportStatus
{
    None,
    Normal,
    Full
};

enum class HydraulicUnbalancedAction
{
    Stop,
    Continue
};

enum class HydraulicCurveType
{
    TankVolume,
    PumpHead,
    PumpEfficiency,
    ValveHeadloss,
    Generic,
    ValveCharacteristic
};

enum class HydraulicControlSimpleType
{
    LowLevel,
    HighLevel,
    Timer,
    TimeOfDay
};

enum class HydraulicControlActionType
{
    Open,
    Close,
    Setting
};

enum class HydraulicControlRuleLogicalOperator
{
    If,
    And,
    Or
};

enum class HydraulicControlRuleObject
{
    Node,
    Link,
    System
};

enum class HydraulicControlRuleVariable
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

enum class HydraulicControlRuleOperator
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

enum class HydraulicControlRuleStatus
{
    Open,
    Closed,
    Active
};

enum class HydraulicSimulationReportSelectionMode
{
    None,
    All,
    Selected
};

enum class HydraulicMapBackdropUnit
{
    None,
    Feet,
    Meters,
    Degrees
};

enum class HydraulicSimulationPumpState
{
    CannotSupplyHead,
    Closed,
    Open,
    CannotSupplyFlow
};

enum class HydraulicSimulationTimestepEventType
{
    ReportStep,
    HydraulicStep,
    QualityStep,
    TankEvent,
    ControlEvent
};

struct HydraulicEntityMetadata
{
    bool enabled = true;
    EntityModelRole model_role = EntityModelRole::Unspecified;
    std::optional<QDate> date_added;
    std::optional<QDate> date_installed;

    QString comment;
    QString tag;
};

struct HydraulicMapPosition
{
    double x = 0.0;
    double y = 0.0;
};

struct HydraulicLinkVertex
{
    CoordinateWGS84 coordinate_wgs84;
    std::optional<HydraulicMapPosition> map_position;
};

struct HydraulicNodeQualitySource
{
    HydraulicNodeQualitySourceType type = HydraulicNodeQualitySourceType::None;
    double strength = 0.0;
    QUuid pattern_uuid;
};

#endif // AOWIS_MODEL_HYDRAULIC_TYPES_H
