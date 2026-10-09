#ifndef AOWIS_MODEL_UNITS_CANONICAL_REGISTRY_H
#define AOWIS_MODEL_UNITS_CANONICAL_REGISTRY_H

#include <array>
#include <string_view>

// Authoritative AOWIS quantity identifiers and canonical UCUM unit identifiers.
// Presentation profiles and alternative unit choices belong to the GUI.
namespace aowis::units
{
struct CanonicalQuantity
{
    std::string_view quantity;
    std::string_view unit;
};

inline constexpr std::array<CanonicalQuantity, 76> canonical_quantities = {{
    {"length", "m"},
    {"elevation", "m"},
    {"altitude", "m"},
    {"distance", "m"},
    {"vertical_offset", "m"},
    {"link_diameter", "mm"},
    {"tank_diameter", "m"},
    {"darcy_weisbach_roughness_height", "mm"},
    {"latitude", "deg"},
    {"longitude", "deg"},
    {"projected_easting", "m"},
    {"projected_northing", "m"},
    {"local_x", "m"},
    {"local_y", "m"},
    {"area", "m2"},
    {"volume", "m3"},
    {"volumetric_flow_rate", "m3/h"},
    {"velocity", "m/s"},
    {"molecular_diffusivity", "m2/s"},
    {"longitudinal_dispersion_coefficient", "m2/s"},
    {"hydraulic_head", "m"},
    {"pressure_head", "m"},
    {"water_level", "m"},
    {"head_gain", "m"},
    {"head_loss", "m"},
    {"head_loss_gradient", "m/km"},
    {"pressure", "kPa"},
    {"stress", "MPa"},
    {"elapsed_time", "s"},
    {"duration", "s"},
    {"time_of_day", "s"},
    {"hazen_williams_roughness_coefficient", "1"},
    {"chezy_manning_roughness_coefficient", "1"},
    {"minor_loss_coefficient", "1"},
    {"darcy_weisbach_friction_factor", "1"},
    {"pump_speed_ratio", "1"},
    {"pattern_multiplier", "1"},
    {"demand_multiplier", "1"},
    {"pressure_exponent", "1"},
    {"emitter_exponent", "1"},
    {"reaction_order", "1"},
    {"specific_gravity", "1"},
    {"relative_viscosity", "1"},
    {"relative_diffusivity", "1"},
    {"peclet_number", "1"},
    {"mixing_fraction", "1"},
    {"hydraulic_accuracy", "1"},
    {"hydraulic_damping_limit", "1"},
    {"relative_error", "1"},
    {"flow_balance_ratio", "1"},
    {"quality_mass_balance_ratio", "1"},
    {"efficiency", "%"},
    {"relative_flow", "%"},
    {"valve_position", "%"},
    {"source_trace_percentage", "%"},
    {"operating_time_percentage", "%"},
    {"demand_reduction_percentage", "%"},
    {"leakage_loss_percentage", "%"},
    {"first_order_bulk_reaction_coefficient", "/d"},
    {"first_order_wall_reaction_coefficient", "m/d"},
    {"leak_area_per_100m_pipe_length", "mm2/(100.m)"},
    {"leak_area_expansion_per_pressure_head", "mm2/m"},
    {"chemical_mass_concentration", "mg/L"},
    {"chemical_amount_concentration", "mmol/L"},
    {"chemical_surface_mass_density", "mg/m2"},
    {"chemical_surface_amount_density", "mmol/m2"},
    {"chemical_mass_flow_rate", "mg/min"},
    {"chemical_amount_flow_rate", "mmol/min"},
    {"water_age", "h"},
    {"power", "kW"},
    {"energy", "kW.h"},
    {"energy_intensity", "kW.h/m3"},
    {"electric_current", "A"},
    {"voltage", "V"},
    {"electrical_resistance", "Ohm"},
    {"capacitance", "F"},
}};

[[nodiscard]] constexpr std::string_view canonicalUnit(std::string_view quantity) noexcept
{
    for (const CanonicalQuantity &entry : canonical_quantities) {
        if (entry.quantity == quantity)
            return entry.unit;
    }
    return {};
}
} // namespace aowis::units

#endif // AOWIS_MODEL_UNITS_CANONICAL_REGISTRY_H
