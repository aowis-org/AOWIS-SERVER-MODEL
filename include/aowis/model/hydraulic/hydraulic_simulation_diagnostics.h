#ifndef AOWIS_MODEL_HYDRAULIC_SIMULATION_DIAGNOSTICS_H
#define AOWIS_MODEL_HYDRAULIC_SIMULATION_DIAGNOSTICS_H

#include <QMetaType>
#include <QString>
#include <QStringList>

#include "hydraulic_simulation_status.h"

enum class HydraulicSimulationDiagnosticSeverity
{
    Information,
    Warning,
    Error,
    Fatal
};

struct HydraulicSimulationDiagnostic
{
    HydraulicSimulationDiagnosticSeverity severity = HydraulicSimulationDiagnosticSeverity::Information;

    HydraulicSimulationStatusStage stage = HydraulicSimulationStatusStage::None;
    HydraulicSimulationStatusOperation operation = HydraulicSimulationStatusOperation::None;
    HydraulicSimulationStatusProperty property = HydraulicSimulationStatusProperty::None;

    HydraulicSimulationStatusEntity entity;

    QString message;
    QStringList details;

    QString backend_name;
    int backend_error_code = 0;
    QString backend_operation;
    QString message_backend;
};

Q_DECLARE_METATYPE(HydraulicSimulationDiagnostic)

#endif // AOWIS_MODEL_HYDRAULIC_SIMULATION_DIAGNOSTICS_H
