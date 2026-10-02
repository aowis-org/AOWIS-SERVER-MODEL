#ifndef AOWIS_MODEL_HYDRAULIC_DEMANDS_H
#define AOWIS_MODEL_HYDRAULIC_DEMANDS_H

#include <QString>
#include <QUuid>

#include "hydraulic_types.h"

struct HydraulicDemand
{
    QString category_name;
    double base_demand_m3_per_h = 0.0;
    HydraulicTimePatternMode pattern_mode = HydraulicTimePatternMode::Constant;
    QUuid pattern_uuid;

    HydraulicDemandSourceMethod source_method = HydraulicDemandSourceMethod::ManualEstimation;
    QString note;
};

#endif // AOWIS_MODEL_HYDRAULIC_DEMANDS_H
