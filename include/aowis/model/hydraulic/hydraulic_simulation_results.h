#ifndef AOWIS_MODEL_HYDRAULIC_SIMULATION_RESULTS_H
#define AOWIS_MODEL_HYDRAULIC_SIMULATION_RESULTS_H

#include <QDateTime>
#include <QList>
#include <QMetaType>
#include <QString>
#include <QUuid>
#include <QtGlobal>

#include "hydraulic_simulation_status.h"
#include "hydraulic_types.h"

struct HydraulicSimulationResultNodeJunction
{
    QString id;
    QUuid uuid;

    double demand_requested_m3_per_h = 0.0;
    double demand_delivered_m3_per_h = 0.0;
    double demand_deficit_m3_per_h = 0.0;

    double emitter_flow_m3_per_h = 0.0;
    double leakage_flow_m3_per_h = 0.0;

    double head_m = 0.0;
    double pressure_head_m = 0.0;

    double quality = 0.0;
    double source_mass_per_minute = 0.0;

    bool appears_in_control = false;
};

struct HydraulicSimulationResultNodeReservoir
{
    QString id;
    QUuid uuid;

    double net_demand_m3_per_h = 0.0;
    double head_m = 0.0;
    double pressure_head_m = 0.0;

    double quality = 0.0;
    double source_mass_per_minute = 0.0;

    bool appears_in_control = false;
};

struct HydraulicSimulationResultNodeTank
{
    QString id;
    QUuid uuid;

    double net_demand_m3_per_h = 0.0;

    double head_m = 0.0;
    double pressure_head_m = 0.0;
    double water_level_m = 0.0;
    double volume_m3 = 0.0;
    double mixing_zone_volume_m3 = 0.0;

    double quality = 0.0;
    double source_mass_per_minute = 0.0;

    bool appears_in_control = false;
};

struct HydraulicSimulationResultLinkPipe
{
    QString id;
    QUuid uuid;

    double flow_m3_per_h = 0.0;
    double leakage_flow_m3_per_h = 0.0;

    double velocity_m_per_s = 0.0;
    double head_loss_m = 0.0;

    bool open = true;
    double setting = 0.0;

    double quality = 0.0;

    bool appears_in_control = false;
};

struct HydraulicSimulationResultLinkPump
{
    QString id;
    QUuid uuid;

    double flow_m3_per_h = 0.0;
    double velocity_m_per_s = 0.0;
    double head_gain_m = 0.0;

    bool open = true;
    HydraulicSimulationPumpState state = HydraulicSimulationPumpState::Closed;
    double speed = 0.0;

    double efficiency_percent = 0.0;
    double power_kw = 0.0;

    double quality = 0.0;

    bool appears_in_control = false;
};

struct HydraulicSimulationResultLinkValve
{
    QString id;
    QUuid uuid;

    double flow_m3_per_h = 0.0;
    double velocity_m_per_s = 0.0;
    double head_loss_m = 0.0;

    bool open = true;
    bool active = false;
    double setting = 0.0;

    double quality = 0.0;

    bool appears_in_control = false;
};

struct HydraulicSimulationResultLinkPumpEnergyUsage
{
    QString pump_id;
    QUuid pump_uuid;

    double time_online_percent = 0.0;
    double average_efficiency_percent = 0.0;
    double average_kw_per_flow_unit = 0.0;
    double average_power_kw = 0.0;
    double peak_power_kw = 0.0;
    double average_cost_per_day = 0.0;
};

struct HydraulicSimulationResultStatistics
{
    qint64 hydraulic_iterations = 0;
    double relative_error = 0.0;
    double maximum_head_error_m = 0.0;
    double maximum_flow_change_m3_per_h = 0.0;

    double quality_mass_balance_ratio = 0.0;

    qint64 deficient_nodes = 0;
    double demand_reduction_percent = 0.0;
    double leakage_loss_percent = 0.0;
};

struct HydraulicSimulationResultTimestepEvent
{
    HydraulicSimulationTimestepEventType type = HydraulicSimulationTimestepEventType::HydraulicStep;
    quint64 time_until_event_s = 0;
    QString tank_id;
    QUuid tank_uuid;
};

struct HydraulicSimulationResult
{
    quint64 time_elapsed_s = 0;

    HydraulicSimulationStatus status;

    QList<HydraulicSimulationResultNodeJunction> nodes_junctions;
    QList<HydraulicSimulationResultNodeReservoir> nodes_reservoirs;
    QList<HydraulicSimulationResultNodeTank> nodes_tanks;

    QList<HydraulicSimulationResultLinkPipe> links_pipes;
    QList<HydraulicSimulationResultLinkPump> links_pumps;
    QList<HydraulicSimulationResultLinkValve> links_valves;

    QList<HydraulicSimulationResultLinkPumpEnergyUsage> links_pump_energy_usage;

    HydraulicSimulationResultStatistics statistics;
    HydraulicSimulationResultTimestepEvent event_next;
};

struct HydraulicSimulationResultTimeline
{
    HydraulicSimulationStatus status;
    QDateTime simulation_start_utc;
    QList<HydraulicSimulationResult> results;
};

Q_DECLARE_METATYPE(HydraulicSimulationResultTimeline)

#endif // AOWIS_MODEL_HYDRAULIC_SIMULATION_RESULTS_H
