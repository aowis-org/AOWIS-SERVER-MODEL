#ifndef SIMULATION_RESULT_H
#define SIMULATION_RESULT_H

#include <QDateTime>
#include <QList>
#include <QMetaType>
#include <QString>

#include "epanet_status.h"
#include "epanet_types.h"

struct JunctionResult
{
    QString id;

    double demand_requested_m3h = 0.0;
    double demand_delivered_m3h = 0.0;
    double demand_deficit_m3h = 0.0;

    double emitter_flow_m3h = 0.0;
    double leakage_flow_m3h = 0.0;

    double head_m = 0.0;
    double pressure_m = 0.0;

    double quality = 0.0;
    double source_mass_per_minute = 0.0;

    bool appears_in_control = false;
};

struct ReservoirResult
{
    QString id;

    double net_demand_m3h = 0.0;
    double head_m = 0.0;
    double pressure_m = 0.0;

    double quality = 0.0;
    double source_mass_per_minute = 0.0;

    bool appears_in_control = false;
};

struct TankResult
{
    QString id;

    double net_demand_m3h = 0.0;

    double head_m = 0.0;
    double pressure_m = 0.0;
    double level_m = 0.0;
    double volume_m3 = 0.0;
    double mixing_zone_volume_m3 = 0.0;

    double quality = 0.0;
    double source_mass_per_minute = 0.0;

    bool appears_in_control = false;
};

struct PipeResult
{
    QString id;

    double flow_lps = 0.0;
    double flow_m3h = 0.0;
    double leakage_flow_m3h = 0.0;

    double velocity_mps = 0.0;
    double headloss = 0.0;

    bool open = true;
    double setting = 0.0;

    double quality = 0.0;

    bool appears_in_control = false;
};

struct PumpResult
{
    QString id;

    double flow_lps = 0.0;
    double flow_m3h = 0.0;
    double velocity_mps = 0.0;
    double headloss = 0.0;

    bool open = true;
    EpanetPumpState state = EpanetPumpState::Closed;
    double speed = 0.0;

    double efficiency_percent = 0.0;
    double power_kw = 0.0;

    double quality = 0.0;

    bool appears_in_control = false;
};

struct ValveResult
{
    QString id;

    double flow_lps = 0.0;
    double flow_m3h = 0.0;
    double velocity_mps = 0.0;
    double headloss = 0.0;

    bool open = true;
    bool active = false;
    double setting = 0.0;

    double quality = 0.0;

    bool appears_in_control = false;
};

struct PumpEnergyUsageResult
{
    QString pump_id;

    double time_online_percent = 0.0;
    double average_efficiency_percent = 0.0;
    double average_kw_per_flow_unit = 0.0;
    double average_power_kw = 0.0;
    double peak_power_kw = 0.0;
    double average_cost_per_day = 0.0;
};

struct EpanetAnalysisStatistics
{
    double hydraulic_iterations = 0.0;
    double relative_error = 0.0;
    double maximum_head_error_m = 0.0;
    double maximum_flow_change_m3h = 0.0;

    double quality_mass_balance_ratio = 0.0;

    double deficient_nodes = 0.0;
    double demand_reduction_percent = 0.0;
    double leakage_loss_percent = 0.0;
};

struct EpanetTimestepEvent
{
    EpanetTimestepEventType type = EpanetTimestepEventType::HydraulicStep;
    long seconds_until_event = 0;
    QString tank_id;
};

struct SimulationResult
{
    long elapsed_time_s = 0;

    EpanetStatus status;

    QList<JunctionResult> junctions;
    QList<ReservoirResult> reservoirs;
    QList<TankResult> tanks;

    QList<PipeResult> pipes;
    QList<PumpResult> pumps;
    QList<ValveResult> valves;

    QList<PumpEnergyUsageResult> pump_energy_usage;

    EpanetAnalysisStatistics statistics;
    EpanetTimestepEvent next_event;
};

struct SimulationResultTimeline
{
    EpanetStatus status;
    QDateTime simulation_start_utc;
    QList<SimulationResult> results;
};

Q_DECLARE_METATYPE(SimulationResultTimeline)

#endif // SIMULATION_RESULT_H
