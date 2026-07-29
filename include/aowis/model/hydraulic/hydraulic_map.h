#ifndef AOWIS_MODEL_HYDRAULIC_MAP_H
#define AOWIS_MODEL_HYDRAULIC_MAP_H

#include <QString>

#include "hydraulic_types.h"

struct HydraulicMapLabel
{
    double x = 0.0;
    double y = 0.0;
    QString text;
    QString anchor_node_id;
};

struct HydraulicMapBackdrop
{
    double lower_left_x = 0.0;
    double lower_left_y = 0.0;
    double upper_right_x = 0.0;
    double upper_right_y = 0.0;

    HydraulicMapBackdropUnit unit = HydraulicMapBackdropUnit::None;
    QString file;

    double offset_x = 0.0;
    double offset_y = 0.0;
};

#endif // AOWIS_MODEL_HYDRAULIC_MAP_H
