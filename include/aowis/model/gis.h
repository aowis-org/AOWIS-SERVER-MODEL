#ifndef GIS_STRUCTS_H
#define GIS_STRUCTS_H

#include <optional>

struct CoordinateWGS84
{
    double lon = 0.0;
    double lat = 0.0;

    // GPS altitude, if available.
    // Not necessarily suitable as hydraulic EPANET elevation.
    std::optional<double> altitude_m;
};

struct CoordinateWGS84Rect
{
    CoordinateWGS84 north_west;
    CoordinateWGS84 south_east;
};

struct CoordinateUTM
{
    double easting = 0.0;
    double northing = 0.0;

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
    double x = 0.0;
    double y = 0.0;
};

#endif // GIS_STRUCTS_H
