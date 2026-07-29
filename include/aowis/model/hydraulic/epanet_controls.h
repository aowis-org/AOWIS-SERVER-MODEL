#ifndef AOWIS_MODEL_HYDRAULIC_EPANET_CONTROLS_H
#define AOWIS_MODEL_HYDRAULIC_EPANET_CONTROLS_H

#include <optional>

#include <QList>
#include <QString>
#include <QUuid>

#include "epanet_types.h"

struct EpanetControlSimple
{
    QString id;
    QUuid uuid;

    EpanetControlSimpleType type = EpanetControlSimpleType::LowLevel;

    QString link_id;
    EpanetControlActionType action = EpanetControlActionType::Setting;
    double setting = 0.0;

    QString trigger_node_id;
    double trigger_level_or_pressure_head_m = 0.0;
    long trigger_time_s = 0;

    bool enabled = true;
};

struct EpanetControlRulePremise
{
    EpanetControlRuleLogicalOperator logical_operator = EpanetControlRuleLogicalOperator::If;
    EpanetControlRuleObject object = EpanetControlRuleObject::Node;
    QString object_id;
    EpanetControlRuleVariable variable = EpanetControlRuleVariable::Pressure;
    EpanetControlRuleOperator comparison = EpanetControlRuleOperator::Greater;

    std::optional<EpanetControlRuleStatus> status;
    std::optional<double> value;
};

struct EpanetControlRuleAction
{
    QString link_id;
    std::optional<EpanetControlRuleStatus> status;
    std::optional<double> setting;
};

struct EpanetControlRule
{
    QString id;
    QUuid uuid;

    QList<EpanetControlRulePremise> premises;
    QList<EpanetControlRuleAction> actions_then;
    QList<EpanetControlRuleAction> actions_else;

    double priority = 0.0;
    bool enabled = true;

    // Preserves exact imported EPANET rule text when desired.
    QString source_text;
};

#endif // AOWIS_MODEL_HYDRAULIC_EPANET_CONTROLS_H
