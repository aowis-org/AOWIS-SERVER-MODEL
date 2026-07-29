#ifndef AOWIS_MODEL_HYDRAULIC_CONTROLS_H
#define AOWIS_MODEL_HYDRAULIC_CONTROLS_H

#include <optional>

#include <QList>
#include <QString>
#include <QUuid>

#include "hydraulic_types.h"

struct HydraulicControlSimple
{
    QString id;
    QUuid uuid;

    HydraulicControlSimpleType type = HydraulicControlSimpleType::LowLevel;

    QString link_id;
    HydraulicControlActionType action = HydraulicControlActionType::Setting;
    double setting = 0.0;

    QString trigger_node_id;
    double trigger_level_or_pressure_head_m = 0.0;
    long trigger_time_s = 0;

    bool enabled = true;
};

struct HydraulicControlRulePremise
{
    HydraulicControlRuleLogicalOperator logical_operator = HydraulicControlRuleLogicalOperator::If;
    HydraulicControlRuleObject object = HydraulicControlRuleObject::Node;
    QString object_id;
    HydraulicControlRuleVariable variable = HydraulicControlRuleVariable::Pressure;
    HydraulicControlRuleOperator comparison = HydraulicControlRuleOperator::Greater;

    std::optional<HydraulicControlRuleStatus> status;
    std::optional<double> value;
};

struct HydraulicControlRuleAction
{
    QString link_id;
    std::optional<HydraulicControlRuleStatus> status;
    std::optional<double> setting;
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
