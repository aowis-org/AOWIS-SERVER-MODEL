#ifndef AOWIS_MODEL_HYDRAULIC_EPANET_OPTIONS_H
#define AOWIS_MODEL_HYDRAULIC_EPANET_OPTIONS_H

#include <optional>
#include <QList>
#include <QString>
#include <QStringList>

#include "epanet_types.h"

struct EpanetOptionsHydraulic
{
    EpanetFlowUnits flow_units = EpanetFlowUnits::LitersPerSecond;
    EpanetPressureUnits pressure_units = EpanetPressureUnits::Meters;
    EpanetHeadlossFormula headloss_formula = EpanetHeadlossFormula::HazenWilliams;
    EpanetDemandModel demand_model = EpanetDemandModel::DemandDriven;

    double minimum_pressure_head_m = 0.0;
    double required_pressure_head_m = 0.0;
    double pressure_exponent = 0.5;

    int maximum_trials = 200;
    double accuracy = 0.001;
    EpanetUnbalancedAction unbalanced_action = EpanetUnbalancedAction::Stop;
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
    EpanetQualityAnalysis analysis = EpanetQualityAnalysis::None;
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

struct EpanetReportField
{
    bool enabled = true;
    std::optional<int> precision;
    std::optional<double> below;
    std::optional<double> above;
};

struct EpanetNodeReportFields
{
    EpanetReportField elevation;
    EpanetReportField demand;
    EpanetReportField head;
    EpanetReportField pressure;
    EpanetReportField quality;
};

struct EpanetLinkReportFields
{
    EpanetReportField length;
    EpanetReportField diameter;
    EpanetReportField flow;
    EpanetReportField velocity;
    EpanetReportField headloss;
    EpanetReportField position;
    EpanetReportField setting;
    EpanetReportField reaction;
    EpanetReportField friction;
};

struct EpanetReportSelectionOptions
{
    EpanetReportSelection selection = EpanetReportSelection::All;
    QStringList ids;
};

struct EpanetOptionsReport
{
    int page_size = 0;
    EpanetStatusReport status = EpanetStatusReport::Normal;
    bool summary = true;
    bool energy = false;

    EpanetReportSelectionOptions nodes;
    EpanetReportSelectionOptions links;

    EpanetNodeReportFields node_fields;
    EpanetLinkReportFields link_fields;

    QStringList additional_commands;
};

struct EpanetMapLabel
{
    double x = 0.0;
    double y = 0.0;
    QString text;
    QString anchor_node_id;
};

struct EpanetBackdrop
{
    double lower_left_x = 0.0;
    double lower_left_y = 0.0;
    double upper_right_x = 0.0;
    double upper_right_y = 0.0;

    EpanetBackdropUnits units = EpanetBackdropUnits::None;
    QString file;

    double offset_x = 0.0;
    double offset_y = 0.0;
};

#endif // AOWIS_MODEL_HYDRAULIC_EPANET_OPTIONS_H
