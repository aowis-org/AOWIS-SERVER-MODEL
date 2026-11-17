#ifndef AOWIS_MODEL_HYDRAULIC_MAP_H
#define AOWIS_MODEL_HYDRAULIC_MAP_H

#include <QString>

#include "../gis.h"

struct HydraulicMapBackdrop
{
    bool enabled = false;

    CoordinateWGS84 lower_left_wgs84;
    CoordinateWGS84 upper_right_wgs84;
    QString file;

    double offset_longitude_deg = 0.0;
    double offset_latitude_deg = 0.0;
};

#endif // AOWIS_MODEL_HYDRAULIC_MAP_H
