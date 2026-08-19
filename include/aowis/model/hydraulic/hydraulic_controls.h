#ifndef AOWIS_MODEL_HYDRAULIC_CONTROLS_H
#define AOWIS_MODEL_HYDRAULIC_CONTROLS_H

#include <optional>

#include <QList>
#include <QString>
#include <QUuid>
#include <QtGlobal>

#include "hydraulic_types.h"

struct HydraulicControlLinkSetting
{
    std::optional<double> pump_speed_ratio;
    std::optional<double> valve_pressure_head_m;
    std::optional<double> valve_flow_m3_per_h;
    std::optional<double> valve_loss_coefficient;
    std::optional<double> valve_position_percent;
};

struct HydraulicControlSimple
{
    QString id;
    QUuid uuid;

    HydraulicControlSimpleType type = HydraulicControlSimpleType::LowLevel;

    QUuid link_uuid;
    HydraulicControlActionType action = HydraulicControlActionType::Setting;
    HydraulicControlLinkSetting setting;

    QUuid trigger_node_uuid;
    double trigger_water_level_m = 0.0;
    double trigger_pressure_head_m = 0.0;
    quint64 trigger_elapsed_time_s = 0;
    quint64 trigger_time_of_day_s = 0;

    bool enabled = true;
};

struct HydraulicControlRulePremise
{
    HydraulicControlRuleLogicalOperator logical_operator = HydraulicControlRuleLogicalOperator::If;
    HydraulicControlRuleObject object = HydraulicControlRuleObject::Node;
    QUuid object_uuid;
    HydraulicControlRuleVariable variable = HydraulicControlRuleVariable::Pressure;
    HydraulicControlRuleOperator comparison = HydraulicControlRuleOperator::Greater;

    std::optional<HydraulicControlRuleStatus> status;

    std::optional<double> demand_m3_per_h;
    std::optional<double> hydraulic_head_m;
    std::optional<double> water_level_m;
    std::optional<double> pressure_head_m;
    std::optional<double> flow_m3_per_h;
    HydraulicControlLinkSetting link_setting;
    std::optional<double> power_kw;
    std::optional<quint64> elapsed_time_s;
    std::optional<quint64> time_of_day_s;
    std::optional<quint64> fill_time_s;
    std::optional<quint64> drain_time_s;
};

struct HydraulicControlRuleAction
{
    QUuid link_uuid;
    std::optional<HydraulicControlRuleStatus> status;
    HydraulicControlLinkSetting setting;
};

struct HydraulicControlRule
{
    QString id;
    QUuid uuid;

    QList<HydraulicControlRulePremise> premises;
    QList<HydraulicControlRuleAction> actions_then;
    QList<HydraulicControlRuleAction> actions_else;

    double priority = 0.0;
    bool enabled = true;

    // Preserves the exact imported backend rule text when desired.
    QString source_text;
};

#endif // AOWIS_MODEL_HYDRAULIC_CONTROLS_H
