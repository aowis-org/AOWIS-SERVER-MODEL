#ifndef AOWIS_MODEL_HYDRAULIC_EPANET_OPTIONS_H
#define AOWIS_MODEL_HYDRAULIC_EPANET_OPTIONS_H

#include <optional>

#include <QString>
#include <QStringList>

#include "epanet_types.h"

struct EpanetOptionsHydraulic
{
    EpanetOptionsHydraulicFlowUnits flow_units = EpanetOptionsHydraulicFlowUnits::LitersPerSecond;
    EpanetOptionsHydraulicPressureUnits pressure_units = EpanetOptionsHydraulicPressureUnits::Meters;
    EpanetOptionsHydraulicHeadlossFormula headloss_formula = EpanetOptionsHydraulicHeadlossFormula::HazenWilliams;
    EpanetOptionsHydraulicDemandModel demand_model = EpanetOptionsHydraulicDemandModel::DemandDriven;

    double minimum_pressure_head_m = 0.0;
    double required_pressure_head_m = 0.0;
    double pressure_exponent = 0.5;

    int maximum_trials = 200;
    double accuracy = 0.001;
    EpanetOptionsHydraulicUnbalancedAction unbalanced_action = EpanetOptionsHydraulicUnbalancedAction::Stop;
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

struct EpanetOptionsQuality
{
    EpanetOptionsQualityAnalysis analysis = EpanetOptionsQualityAnalysis::None;
    QString chemical_name;
    QString chemical_units;
    QString trace_node_id;

    double tolerance = 0.01;
    double relative_diffusivity = 1.0;
};

struct EpanetOptionsReaction
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

struct EpanetOptionsEnergy
{
    double global_pump_efficiency_percent = 75.0;
    double global_energy_price_per_kw_h = 0.0;
    QString global_energy_price_pattern_id;
    double demand_charge_per_kw = 0.0;
};

struct EpanetOptionsReportField
{
    bool enabled = true;
    std::optional<int> precision;
    std::optional<double> below;
    std::optional<double> above;
};

struct EpanetOptionsReportFieldsNode
{
    EpanetOptionsReportField elevation;
    EpanetOptionsReportField demand;
    EpanetOptionsReportField head;
    EpanetOptionsReportField pressure;
    EpanetOptionsReportField quality;
};

struct EpanetOptionsReportFieldsLink
{
    EpanetOptionsReportField length;
    EpanetOptionsReportField diameter;
    EpanetOptionsReportField flow;
    EpanetOptionsReportField velocity;
    EpanetOptionsReportField headloss;
    EpanetOptionsReportField position;
    EpanetOptionsReportField setting;
    EpanetOptionsReportField reaction;
    EpanetOptionsReportField friction;
};

struct EpanetOptionsReportSelection
{
    EpanetOptionsReportSelectionMode mode = EpanetOptionsReportSelectionMode::All;
    QStringList ids;
};

struct EpanetOptionsReport
{
    int page_size = 0;
    EpanetOptionsReportStatus status = EpanetOptionsReportStatus::Normal;
    bool summary = true;
    bool energy = false;

    EpanetOptionsReportSelection selection_nodes;
    EpanetOptionsReportSelection selection_links;

    EpanetOptionsReportFieldsNode fields_node;
    EpanetOptionsReportFieldsLink fields_link;

    QStringList additional_commands;
};

struct EpanetMapLabel
{
    double x = 0.0;
    double y = 0.0;
    QString text;
    QString anchor_node_id;
};

struct EpanetMapBackdrop
{
    double lower_left_x = 0.0;
    double lower_left_y = 0.0;
    double upper_right_x = 0.0;
    double upper_right_y = 0.0;

    EpanetMapBackdropUnits units = EpanetMapBackdropUnits::None;
    QString file;

    double offset_x = 0.0;
    double offset_y = 0.0;
};

#endif // AOWIS_MODEL_HYDRAULIC_EPANET_OPTIONS_H
