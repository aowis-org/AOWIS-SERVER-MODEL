#ifndef AOWIS_MODEL_HYDRAULIC_SIMULATION_OPTIONS_H
#define AOWIS_MODEL_HYDRAULIC_SIMULATION_OPTIONS_H

#include <optional>

#include <QList>
#include <QString>
#include <QStringList>
#include <QUuid>

#include "hydraulic_types.h"

struct HydraulicSolverOptions
{
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
    QUuid default_demand_pattern_uuid;

    bool emitters_can_backflow = false;

    double specific_gravity = 1.0;
    double relative_viscosity = 1.0;
};

struct WaterQualitySolverOptions
{
    WaterQualityAnalysisType analysis = WaterQualityAnalysisType::None;
    QString chemical_name;
    QUuid trace_node_uuid;

    // Only the tolerance matching analysis is used. Chemical concentration is
    // canonical mg/L, water age is h, and source trace is %.
    double chemical_tolerance_mg_per_l = 0.01;
    double water_age_tolerance_h = 0.01;
    double source_trace_tolerance_percent = 0.01;

    double relative_diffusivity = 1.0;
};

struct WaterQualityReactionOptions
{
    WaterQualityBulkReaction global_pipe_bulk_reaction;
    WaterQualityWallReaction global_pipe_wall_reaction;
    WaterQualityBulkReaction global_tank_bulk_reaction;

    double limiting_concentration_mg_per_l = 0.0;

    // EPANET's roughness-reaction correlation factor is backend/formula dependent.
    double roughness_reaction_factor = 0.0;
};

struct PumpEnergyOptions
{
    // ISO 4217 alphabetic code for all energy price, demand-charge, and cost values.
    // Currency conversion and exchange rates are controller responsibilities.
    QString currency_iso4217;

    double global_pump_efficiency_percent = 75.0;
    double global_energy_price_per_kw_h = 0.0;
    QUuid global_energy_price_pattern_uuid;
    double demand_charge_per_kw = 0.0;
};

struct HydraulicSimulationReportField
{
    bool enabled = true;
    std::optional<int> precision;
};

struct HydraulicSimulationReportFieldM
{
    bool enabled = true;
    std::optional<int> precision;
    std::optional<double> below_m;
    std::optional<double> above_m;
};

struct HydraulicSimulationReportFieldM3PerH
{
    bool enabled = true;
    std::optional<int> precision;
    std::optional<double> below_m3_per_h;
    std::optional<double> above_m3_per_h;
};

struct HydraulicSimulationReportFieldMm
{
    bool enabled = true;
    std::optional<int> precision;
    std::optional<double> below_mm;
    std::optional<double> above_mm;
};

struct HydraulicSimulationReportFieldMPerS
{
    bool enabled = true;
    std::optional<int> precision;
    std::optional<double> below_m_per_s;
    std::optional<double> above_m_per_s;
};

struct HydraulicSimulationReportFieldMPerKm
{
    bool enabled = true;
    std::optional<int> precision;
    std::optional<double> below_m_per_km;
    std::optional<double> above_m_per_km;
};

struct HydraulicSimulationReportFieldFrictionFactor
{
    bool enabled = true;
    std::optional<int> precision;
    std::optional<double> below_friction_factor;
    std::optional<double> above_friction_factor;
};

struct HydraulicSimulationReportNodeFields
{
    HydraulicSimulationReportFieldM elevation;
    HydraulicSimulationReportFieldM3PerH demand;
    HydraulicSimulationReportFieldM head;
    HydraulicSimulationReportFieldM pressure;
    HydraulicSimulationReportField quality;
};

struct HydraulicSimulationReportLinkFields
{
    HydraulicSimulationReportFieldM length;
    HydraulicSimulationReportFieldMm diameter;
    HydraulicSimulationReportFieldM3PerH flow;
    HydraulicSimulationReportFieldMPerS velocity;
    HydraulicSimulationReportFieldMPerKm headloss;
    HydraulicSimulationReportField position;
    HydraulicSimulationReportField setting;
    HydraulicSimulationReportField reaction;
    HydraulicSimulationReportFieldFrictionFactor friction;
};

struct HydraulicSimulationReportSelection
{
    HydraulicSimulationReportSelectionMode mode = HydraulicSimulationReportSelectionMode::All;
    QList<QUuid> uuids;
};

struct HydraulicSimulationReportOptions
{
    int page_size = 0;
    HydraulicSimulationReportStatus status = HydraulicSimulationReportStatus::Normal;
    bool summary = true;
    bool messages = true;
    bool energy = false;

    HydraulicSimulationReportSelection selection_nodes;
    HydraulicSimulationReportSelection selection_links;

    HydraulicSimulationReportNodeFields fields_node;
    HydraulicSimulationReportLinkFields fields_link;

    // Commands that are understood only by the selected simulation backend.
    QStringList backend_commands;
};

#endif // AOWIS_MODEL_HYDRAULIC_SIMULATION_OPTIONS_H
