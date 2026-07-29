#ifndef AOWIS_MODEL_GIS_H
#define AOWIS_MODEL_GIS_H

#include <optional>

struct CoordinateWGS84
{
    double longitude_deg = 0.0;
    double latitude_deg = 0.0;

    // GPS altitude, if available.
    // Not necessarily suitable as hydraulic model elevation.
    std::optional<double> altitude_m;
};

struct CoordinateWGS84Rect
{
    CoordinateWGS84 north_west;
    CoordinateWGS84 south_east;
};

struct CoordinateUTM
{
    double easting_m = 0.0;
    double northing_m = 0.0;

    int zone = 0;
    bool hemisphere_northern = true;

    bool isUPS() const
    {
        return this->zone == 0;
    }

    bool isUTM() const
    {
        return this->zone >= 1 && this->zone <= 60;
    }
};

// UTM coordinates relative to a user-defined project origin.
struct CoordinateLocal
{
    double x_m = 0.0;
    double y_m = 0.0;
};

#endif // AOWIS_MODEL_GIS_H
