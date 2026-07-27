#ifndef AOWIS_MODEL_HYDRAULIC_EPANET_CONTROLS_H
#define AOWIS_MODEL_HYDRAULIC_EPANET_CONTROLS_H

#include <optional>
#include <QList>
#include <QString>
#include <QUuid>

#include "epanet_types.h"

struct EpanetSimpleControl
{
    QString id;
    QUuid uuid;

    EpanetSimpleControlType type = EpanetSimpleControlType::LowLevel;

    QString link_id;
    EpanetControlActionType action = EpanetControlActionType::Setting;
    double setting = 0.0;

    QString trigger_node_id;
    double trigger_level_or_pressure_m = 0.0;
    long trigger_time_s = 0;

    bool enabled = true;
};

struct EpanetRulePremise
{
    EpanetRuleLogicalOperator logical_operator = EpanetRuleLogicalOperator::If;
    EpanetRuleObject object = EpanetRuleObject::Node;
    QString object_id;
    EpanetRuleVariable variable = EpanetRuleVariable::Pressure;
    EpanetRuleOperator comparison = EpanetRuleOperator::Greater;

    std::optional<EpanetRuleStatus> status;
    std::optional<double> value;
};

struct EpanetRuleAction
{
    QString link_id;
    std::optional<EpanetRuleStatus> status;
    std::optional<double> setting;
};

struct EpanetRule
{
    QString id;
    QUuid uuid;

    QList<EpanetRulePremise> premises;
    QList<EpanetRuleAction> then_actions;
    QList<EpanetRuleAction> else_actions;

    double priority = 0.0;
    bool enabled = true;

    // Preserves exact imported EPANET rule text when desired.
    QString source_text;
};

#endif // AOWIS_MODEL_HYDRAULIC_EPANET_CONTROLS_H
