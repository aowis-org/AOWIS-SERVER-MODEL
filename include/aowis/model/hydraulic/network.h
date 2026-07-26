#ifndef NETWORK_HYDRAULIC_H
#define NETWORK_HYDRAULIC_H

#include <optional>

#include <QList>
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

enum class ValveType
{
    PRV, // Pressure Reducing Valve
    PSV, // Pressure Sustaining Valve
    FCV, // Flow Control Valve
    PBV, // Pressure Breaker Valve
    TCV, // Throttle Control Valve
    GPV, // General Purpose Valve
    PCV  // Positional Control Valve, EPANET 2.3+
};

enum class ValveInitialStatus
{
    Active,
    Open,
    Closed
};

struct TankVolumeCurvePoint
{
    double level_m = 0.0;
    double volume_m3 = 0.0;
};

struct TankVolumeCurve
{
    QString id;
    QUuid uuid;
    QList<TankVolumeCurvePoint> points;
};

struct PumpCurvePoint
{
    double flow_m3h = 0.0;
    double head_m = 0.0;
};

struct PumpCurve
{
    QString id;
    QUuid uuid;
    QList<PumpCurvePoint> points;
};

struct JunctionDemand
{
    double base_demand_m3h = 0.0;
    QString pattern_id;
    DemandSourceMethod source_method = DemandSourceMethod::ManualEstimation;
    QString note;
};

struct Tank
{
    QString id;
    QUuid uuid;
    
    double latitude = 0.0;
    double longitude = 0.0;
    
    ElevationInputType elevation_input_type = ElevationInputType::BottomElevation;
    double bottom_elevation_m = 0.0;
    double terrain_elevation_m = 0.0;
    double bottom_offset_m = 0.0;
    
    double initial_level_m = 0.0;
    double minimum_level_m = 0.0;
    double maximum_level_m = 0.0;
    
    TankGeometryInputType geometry_input_type = TankGeometryInputType::Cylindrical;
    double diameter_m = 0.0;
    double cross_section_area_m2 = 0.0;
    double volume_at_maximum_level_m3 = 0.0;
    double minimum_volume_m3 = 0.0;
    QString volume_curve_id;
    
    bool can_overflow = false;
    
    QualitySourceType quality_source_type = QualitySourceType::None;
    TankMixingModel mixing_model = TankMixingModel::CompleteMix;
};

struct Reservoir
{
    QString id;
    QUuid uuid;
    
    double latitude = 0.0;
    double longitude = 0.0;
    
    ElevationInputType head_input_type = ElevationInputType::TotalHead;
    double head_m = 0.0;
    double terrain_elevation_m = 0.0;
    double head_offset_m = 0.0;
    QString head_pattern_id;
};

struct Junction
{
    QString id;
    QUuid uuid;
    
    double latitude = 0.0;
    double longitude = 0.0;
    
    ElevationInputType elevation_input_type = ElevationInputType::TotalElevation;
    double elevation_m = 0.0;
    double terrain_elevation_m = 0.0;
    double elevation_offset_m = 0.0;
    
    QList<JunctionDemand> demands;
};

struct Pipe
{
    QString id;
    QUuid uuid;
    
    QString node_id_from;
    QString node_id_to;
    
    double length_calculated_m = 0.0;
    std::optional<double> length_measured_m;
    
    PipeInitialStatus initial_status = PipeInitialStatus::Open;
    double diameter_mm = 100.0;
    
    QString material_id;
    double roughness_hw = 130.0;
    double roughness_dw_mm = 0.1;
    double roughness_cm = 0.013;
    double minor_loss = 0.0;
    
    bool override_reaction_coefficients = false;
    double bulk_reaction_coefficient_per_day = 0.0;
    double wall_reaction_coefficient_m_per_day = 0.0;
};

struct Pump
{
    QString id;
    QUuid uuid;
    
    QString node_id_from;
    QString node_id_to;
    
    PumpDefinitionType definition_type = PumpDefinitionType::ConstantPower;
    double constant_power_kw = 0.0;
    QString head_curve_id;
    
    double initial_speed = 1.0;
    PumpInitialStatus initial_status = PumpInitialStatus::On;
    QString speed_pattern_id;
    
    PumpControlType control_type = PumpControlType::None;
    
    double constant_efficiency_percent = 75.0;
    QString efficiency_curve_id;
    double energy_price_per_kwh = 0.0;
    QString price_pattern_id;
};

struct Valve
{
    QString id;
    QUuid uuid;
    
    QString node_id_from;
    QString node_id_to;
    
    ValveType type = ValveType::PRV;
    double setting = 0.0;
    QString setting_curve_id;
    
    ValveInitialStatus initial_status = ValveInitialStatus::Active;
    double diameter_mm = 0.0;
    double minor_loss = 0.0;
};

struct CustomerPoint
{
    QString id;
    QUuid uuid;
};

struct NetworkHydraulic
{
    QUuid uuid;
    
    long duration_s = 0;
    long hydraulic_timestep_s = 3600;
    
    QList<Reservoir> reservoirs;
    QList<Junction> junctions;
    
    QList<Tank> tanks;
    QList<TankVolumeCurve> tank_volume_curves;
    
    QList<Pipe> pipes;
    QList<Pump> pumps;
    QList<PumpCurve> pump_curves;
    QList<Valve> valves;
    
    QList<CustomerPoint> customer_points;
};

#endif // NETWORK_HYDRAULIC_H
