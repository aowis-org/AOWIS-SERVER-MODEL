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

struct HydraulicLinkVertex
{
    CoordinateWGS84 coordinate_wgs84;
};

struct HydraulicNodeQualitySource
{
    HydraulicNodeQualitySourceType type = HydraulicNodeQualitySourceType::None;

    // Concentration, flow-paced, and setpoint sources use concentration.
    double chemical_concentration_mg_per_l = 0.0;

    // Mass boosters use an absolute chemical mass flow.
    double chemical_mass_flow_mg_per_min = 0.0;

    QUuid pattern_uuid;
};

struct WaterQualityBulkReaction
{
    // Reaction rate = coefficient * concentration^order. The coefficient therefore
    // has no fixed canonical UCUM unit independent of order.
    double coefficient = 0.0;
    double order = 1.0;
};

struct WaterQualityWallReaction
{
    // Wall-reaction coefficient dimensions depend on the configured reaction order.
    double coefficient = 0.0;
    double order = 1.0;
};

#endif // AOWIS_MODEL_HYDRAULIC_TYPES_H
