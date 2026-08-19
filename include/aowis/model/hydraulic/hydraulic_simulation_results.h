#ifndef AOWIS_MODEL_HYDRAULIC_SIMULATION_RESULTS_H
#define AOWIS_MODEL_HYDRAULIC_SIMULATION_RESULTS_H

#include <optional>

#include <QDateTime>
#include <QList>
#include <QMetaType>
#include <QString>
#include <QUuid>
#include <QtGlobal>

#include "hydraulic_simulation_diagnostics.h"
#include "hydraulic_simulation_status.h"
#include "hydraulic_types.h"

struct HydraulicSimulationResultNodeJunction
{
    QString id;
    QUuid uuid;

    double demand_requested_m3_per_h = 0.0;
    double demand_delivered_m3_per_h = 0.0;
    double demand_deficit_m3_per_h = 0.0;

    // Total hydraulic outflow at the junction: delivered consumer demand,
    // emitter flow, and pipe leakage assigned to the node.
    double total_demand_m3_per_h = 0.0;

    double emitter_flow_m3_per_h = 0.0;
    double leakage_flow_m3_per_h = 0.0;

    double hydraulic_head_m = 0.0;
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
    double hydraulic_head_m = 0.0;
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

    double hydraulic_head_m = 0.0;
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
    double head_loss_gradient_m_per_km = 0.0;
    double friction_factor = 0.0;

    bool open = true;
    std::optional<double> roughness_hazen_williams;
    std::optional<double> roughness_darcy_weisbach_mm;
    std::optional<double> roughness_chezy_manning;

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
    double speed_ratio = 0.0;

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

struct HydraulicSimulationResultFlowBalance
{
    double total_inflow_m3_per_h = 0.0;
    double total_outflow_m3_per_h = 0.0;
    double consumer_demand_m3_per_h = 0.0;
    double demand_deficit_m3_per_h = 0.0;
    double emitter_flow_m3_per_h = 0.0;
    double leakage_flow_m3_per_h = 0.0;

    // Positive storage flow means tanks are filling; negative storage flow
    // means tanks are supplying the network.
    double storage_flow_m3_per_h = 0.0;
    double flow_balance_ratio = 0.0;
};

struct HydraulicSimulationResultEnergyUsage
{
    // Peak simultaneous power demand across all pumps.
    double peak_power_kw = 0.0;
    double energy_cost_per_day = 0.0;
    double demand_charge_per_day = 0.0;
    double total_cost_per_day = 0.0;
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
    QString control_id;
    QUuid control_uuid;
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

    // Full-run summaries are populated on the final result in the timeline.
    HydraulicSimulationResultFlowBalance flow_balance;
    HydraulicSimulationResultEnergyUsage energy_usage;

    HydraulicSimulationResultStatistics statistics;
    HydraulicSimulationResultTimestepEvent event_next;
};

// Numerical result trustworthiness is independent from the overall backend status.
// A backend can fail during reporting/cleanup after producing valid numerical results.
enum class HydraulicSimulationResultValidity
{
    Valid,
    Partial,
    Invalid
};

Q_DECLARE_METATYPE(HydraulicSimulationResultValidity)

struct HydraulicSimulationResultTimeline
{
    HydraulicSimulationStatus status;
    HydraulicSimulationResultValidity validity = HydraulicSimulationResultValidity::Invalid;
    QList<HydraulicSimulationDiagnostic> diagnostics;
    QDateTime simulation_start_utc;
    QList<HydraulicSimulationResult> results;
};

Q_DECLARE_METATYPE(HydraulicSimulationResultTimeline)

#endif // AOWIS_MODEL_HYDRAULIC_SIMULATION_RESULTS_H
