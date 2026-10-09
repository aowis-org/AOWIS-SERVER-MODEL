#ifndef AOWIS_MODEL_UNITS_CONVERSION_H
#define AOWIS_MODEL_UNITS_CONVERSION_H

// Physical unit definitions only. Source-format interpretation belongs in adapters.
// Decimal definitions below are exact by international agreement; floating-point
// evaluation is subject to ordinary IEEE-754 rounding.
namespace aowis::units
{
inline constexpr double metres_per_international_foot = 0.3048;
inline constexpr double metres_per_us_survey_foot = 1200.0 / 3937.0;
inline constexpr double metres_per_inch = 0.0254;
inline constexpr double cubic_metres_per_us_gallon = 0.003785411784;
inline constexpr double cubic_metres_per_imperial_gallon = 0.00454609;
inline constexpr double pascals_per_psi = 6894.757293168361; // Derived from exact lbf/in² definitions.
inline constexpr double pascals_per_kilopascal = 1000.0;
inline constexpr double pascals_per_bar = 100000.0;
inline constexpr double seconds_per_hour = 3600.0;
inline constexpr double seconds_per_day = 86400.0;
inline constexpr double cubic_metres_per_cubic_foot =
    metres_per_international_foot * metres_per_international_foot * metres_per_international_foot;
inline constexpr double cubic_metres_per_acre_foot =
    43560.0 * cubic_metres_per_cubic_foot;

[[nodiscard]] constexpr double feetToMetres(double value) noexcept
{
    return value * metres_per_international_foot;
}
[[nodiscard]] constexpr double usSurveyFeetToMetres(double value) noexcept
{
    return value * metres_per_us_survey_foot;
}
[[nodiscard]] constexpr double inchesToMillimetres(double value) noexcept
{
    return value * (metres_per_inch * 1000.0);
}
[[nodiscard]] constexpr double feetToMillimetres(double value) noexcept
{
    return feetToMetres(value) * 1000.0;
}
[[nodiscard]] constexpr double cubicFeetToCubicMetres(double value) noexcept
{
    return value * cubic_metres_per_cubic_foot;
}
[[nodiscard]] constexpr double usGallonsToCubicMetres(double value) noexcept
{
    return value * cubic_metres_per_us_gallon;
}
[[nodiscard]] constexpr double cubicMetresPerSecondToCubicMetresPerHour(double value) noexcept
{
    return value * seconds_per_hour;
}
[[nodiscard]] constexpr double litresPerSecondToCubicMetresPerHour(double value) noexcept
{
    return value * (seconds_per_hour / 1000.0);
}
[[nodiscard]] constexpr double litresPerMinuteToCubicMetresPerHour(double value) noexcept
{
    return value * (60.0 / 1000.0);
}
[[nodiscard]] constexpr double cubicMetresPerDayToCubicMetresPerHour(double value) noexcept
{
    return value / 24.0;
}
[[nodiscard]] constexpr double millionLitresPerDayToCubicMetresPerHour(double value) noexcept
{
    return value * (1000.0 / 24.0);
}
[[nodiscard]] constexpr double cubicFeetPerSecondToCubicMetresPerHour(double value) noexcept
{
    return value * cubic_metres_per_cubic_foot * seconds_per_hour;
}
[[nodiscard]] constexpr double usGallonsPerMinuteToCubicMetresPerHour(double value) noexcept
{
    return value * cubic_metres_per_us_gallon * 60.0;
}
[[nodiscard]] constexpr double millionUsGallonsPerDayToCubicMetresPerHour(double value) noexcept
{
    return value * 1000000.0 * cubic_metres_per_us_gallon / 24.0;
}
[[nodiscard]] constexpr double millionImperialGallonsPerDayToCubicMetresPerHour(double value) noexcept
{
    return value * 1000000.0 * cubic_metres_per_imperial_gallon / 24.0;
}
[[nodiscard]] constexpr double acreFeetPerDayToCubicMetresPerHour(double value) noexcept
{
    return value * cubic_metres_per_acre_foot / 24.0;
}
[[nodiscard]] constexpr double psiToPascals(double value) noexcept
{
    return value * pascals_per_psi;
}
[[nodiscard]] constexpr double kilopascalsToPascals(double value) noexcept
{
    return value * pascals_per_kilopascal;
}
[[nodiscard]] constexpr double barToPascals(double value) noexcept
{
    return value * pascals_per_bar;
}
// The caller supplies actual fluid density (kg/m³) and gravitational
// acceleration (m/s²); neither is implicitly fixed to an EPANET convention.
[[nodiscard]] constexpr double pascalsToMetresHead(
    double pressure_pascals, double density_kg_per_m3, double gravity_m_per_s2) noexcept
{
    return pressure_pascals / (density_kg_per_m3 * gravity_m_per_s2);
}
}

#endif
