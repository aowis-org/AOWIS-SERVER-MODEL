#ifndef AOWIS_MODEL_HYDRAULIC_SIMULATION_OPTIONS_H
#define AOWIS_MODEL_HYDRAULIC_SIMULATION_OPTIONS_H

#include <optional>

#include <QString>
#include <QStringList>

#include "hydraulic_types.h"

struct HydraulicSolverOptions
{
    HydraulicFlowUnit flow_unit = HydraulicFlowUnit::LitersPerSecond;
    HydraulicPressureUnit pressure_unit = HydraulicPressureUnit::Meters;
    HydraulicHeadlossFormula headloss_formula = HydraulicHeadlossFormula::HazenWilliams;
    HydraulicDemandModel demand_model = HydraulicDemandModel::DemandDriven;

    double minimum_pressure_head_m = 0.0;
    double required_pressure_head_m = 0.0;
    double pressure_exponent = 0.5;

    int maximum_trials = 200;
    double accuracy = 0.001;
    HydraulicUnbalancedAction unbalanced_action = HydraulicUnbalancedAction::Stop;
    int unbalanced_extra_trials = 0;
    int check_frequency = 2;
    int maximum_check = 10;
    double damping_limit = 0.0;
    double maximum_head_error_m = 0.0;
    double maximum_flow_change_m3_per_h = 0.0;

    double demand_multiplier = 1.0;
    QString default_demand_pattern_id;

    double emitter_exponent = 0.5;
    bool emitters_can_backflow = false;

    double specific_gravity = 1.0;
    double relative_viscosity = 1.0;
};

struct WaterQualitySolverOptions
{
    WaterQualityAnalysisType analysis = WaterQualityAnalysisType::None;
    QString chemical_name;
    QString chemical_units;
    QString trace_node_id;

    double tolerance = 0.01;
    double relative_diffusivity = 1.0;
};

struct WaterQualityReactionOptions
{
    double pipe_bulk_order = 1.0;
    double pipe_wall_order = 1.0;
    double tank_bulk_order = 1.0;

    double global_pipe_bulk_coefficient_per_day = 0.0;
    double global_pipe_wall_coefficient_m_per_day = 0.0;
    double global_tank_bulk_coefficient_per_day = 0.0;

    double limiting_concentration = 0.0;
    double roughness_correlation = 0.0;
};

struct PumpEnergyOptions
{
    double global_pump_efficiency_percent = 75.0;
    double global_energy_price_per_kw_h = 0.0;
    QString global_energy_price_pattern_id;
    double demand_charge_per_kw = 0.0;
};

struct HydraulicSimulationReportField
{
    bool enabled = true;
    std::optional<int> precision;
    std::optional<double> below;
    std::optional<double> above;
};

struct HydraulicSimulationReportNodeFields
{
    HydraulicSimulationReportField elevation;
    HydraulicSimulationReportField demand;
    HydraulicSimulationReportField head;
    HydraulicSimulationReportField pressure;
    HydraulicSimulationReportField quality;
};

struct HydraulicSimulationReportLinkFields
{
    HydraulicSimulationReportField length;
    HydraulicSimulationReportField diameter;
    HydraulicSimulationReportField flow;
    HydraulicSimulationReportField velocity;
    HydraulicSimulationReportField headloss;
    HydraulicSimulationReportField position;
    HydraulicSimulationReportField setting;
    HydraulicSimulationReportField reaction;
    HydraulicSimulationReportField friction;
};

struct HydraulicSimulationReportSelection
{
    HydraulicSimulationReportSelectionMode mode = HydraulicSimulationReportSelectionMode::All;
    QStringList ids;
};

struct HydraulicSimulationReportOptions
{
    int page_size = 0;
    HydraulicSimulationReportStatus status = HydraulicSimulationReportStatus::Normal;
    bool summary = true;
    bool energy = false;

    HydraulicSimulationReportSelection selection_nodes;
    HydraulicSimulationReportSelection selection_links;

    HydraulicSimulationReportNodeFields fields_node;
    HydraulicSimulationReportLinkFields fields_link;

    // Commands that are understood only by the selected simulation backend.
    QStringList backend_commands;
};

#endif // AOWIS_MODEL_HYDRAULIC_SIMULATION_OPTIONS_H
