#ifndef AOWIS_MODEL_UNITS_UNIT_PROFILE_H
#define AOWIS_MODEL_UNITS_UNIT_PROFILE_H

#include "canonical_registry.h"
#include <array>
#include <utility>
#include <map>
#include <string>
#include <string_view>
#include <vector>

// Qt-independent presentation-unit metadata and profile definitions.
// None of these profiles alter canonical values in the hydraulic model.
namespace aowis::units {
struct UnitField {
    std::string_view group;
    std::string_view key;
    std::string_view choices; // Semicolon-separated UCUM/display identifiers.
};

inline constexpr std::array<UnitField, 76> unit_fields = {{
    {"Lengths and geometry", "length", "m;ft;km;mi"},
    {"Lengths and geometry", "elevation", "m;ft"},
    {"Lengths and geometry", "altitude", "m;ft"},
    {"Lengths and geometry", "distance", "m;ft;km;mi"},
    {"Lengths and geometry", "vertical_offset", "m;ft"},
    {"Lengths and geometry", "link_diameter", "mm;in;cm;m"},
    {"Lengths and geometry", "tank_diameter", "m;ft"},
    {"Lengths and geometry", "darcy_weisbach_roughness_height", "mm;in;millift"},
    {"Coordinates", "latitude", "deg"},
    {"Coordinates", "longitude", "deg"},
    {"Coordinates", "projected_easting", "m;ft"},
    {"Coordinates", "projected_northing", "m;ft"},
    {"Coordinates", "local_x", "m;ft"},
    {"Coordinates", "local_y", "m;ft"},
    {"Area and volume", "area", "m2;ft2;ha;acre"},
    {"Area and volume", "volume", "m3;L;ft3;US gal;Imp gal"},
    {"Flow and transport", "volumetric_flow_rate", "m3/h;L/s;L/min;ML/d;m3/d;m3/s;ft3/s;US gal/min;MUSgal/d;MImpgal/d;acre.ft/d"},
    {"Flow and transport", "velocity", "m/s;ft/s"},
    {"Flow and transport", "molecular_diffusivity", "m2/s;cm2/s"},
    {"Flow and transport", "longitudinal_dispersion_coefficient", "m2/s;cm2/s"},
    {"Hydraulics and pressure", "hydraulic_head", "m;ft"},
    {"Hydraulics and pressure", "pressure_head", "m;ft"},
    {"Hydraulics and pressure", "water_level", "m;ft"},
    {"Hydraulics and pressure", "head_gain", "m;ft"},
    {"Hydraulics and pressure", "head_loss", "m;ft"},
    {"Hydraulics and pressure", "head_loss_gradient", "m/km;ft/1000ft"},
    {"Hydraulics and pressure", "pressure", "kPa;Pa;bar;psi"},
    {"Hydraulics and pressure", "stress", "MPa;kPa;psi"},
    {"Time", "elapsed_time", "s;min;h;d"},
    {"Time", "duration", "s;min;h;d"},
    {"Time", "time_of_day", "s;min;h"},
    {"Dimensionless", "hazen_williams_roughness_coefficient", "1"},
    {"Dimensionless", "chezy_manning_roughness_coefficient", "1"},
    {"Dimensionless", "minor_loss_coefficient", "1"},
    {"Dimensionless", "darcy_weisbach_friction_factor", "1"},
    {"Dimensionless", "pump_speed_ratio", "1"},
    {"Dimensionless", "pattern_multiplier", "1"},
    {"Dimensionless", "demand_multiplier", "1"},
    {"Dimensionless", "pressure_exponent", "1"},
    {"Dimensionless", "emitter_exponent", "1"},
    {"Dimensionless", "reaction_order", "1"},
    {"Dimensionless", "specific_gravity", "1"},
    {"Dimensionless", "relative_viscosity", "1"},
    {"Dimensionless", "relative_diffusivity", "1"},
    {"Dimensionless", "peclet_number", "1"},
    {"Dimensionless", "mixing_fraction", "1"},
    {"Dimensionless", "hydraulic_accuracy", "1"},
    {"Dimensionless", "hydraulic_damping_limit", "1"},
    {"Dimensionless", "relative_error", "1"},
    {"Dimensionless", "flow_balance_ratio", "1"},
    {"Dimensionless", "quality_mass_balance_ratio", "1"},
    {"Percentages", "efficiency", "%;1"},
    {"Percentages", "relative_flow", "%;1"},
    {"Percentages", "valve_position", "%;1"},
    {"Percentages", "source_trace_percentage", "%;1"},
    {"Percentages", "operating_time_percentage", "%;1"},
    {"Percentages", "demand_reduction_percentage", "%;1"},
    {"Percentages", "leakage_loss_percentage", "%;1"},
    {"Reactions and leakage", "first_order_bulk_reaction_coefficient", "/d;/h;/s"},
    {"Reactions and leakage", "first_order_wall_reaction_coefficient", "m/d;ft/d"},
    {"Reactions and leakage", "leak_area_per_100m_pipe_length", "mm2/(100.m);in2/(100.ft)"},
    {"Reactions and leakage", "leak_area_expansion_per_pressure_head", "mm2/m;in2/ft"},
    {"Water quality", "chemical_mass_concentration", "mg/L;g/m3;ug/L"},
    {"Water quality", "chemical_amount_concentration", "mmol/L;mol/m3"},
    {"Water quality", "chemical_surface_mass_density", "mg/m2;g/m2"},
    {"Water quality", "chemical_surface_amount_density", "mmol/m2;mol/m2"},
    {"Water quality", "chemical_mass_flow_rate", "mg/min;g/min;g/h"},
    {"Water quality", "chemical_amount_flow_rate", "mmol/min;mol/min"},
    {"Water quality", "water_age", "h;d;min"},
    {"Power and electrical", "power", "kW;W;hp"},
    {"Power and electrical", "energy", "kW.h;J;MJ"},
    {"Power and electrical", "energy_intensity", "kW.h/m3;kW.h/ft3"},
    {"Power and electrical", "electric_current", "A;mA"},
    {"Power and electrical", "voltage", "V;mV;kV"},
    {"Power and electrical", "electrical_resistance", "Ohm;kOhm"},
    {"Power and electrical", "capacitance", "F;uF"},
}};

inline constexpr std::array<std::string_view, 11> flow_template_names = {{
    "CMH", "LPS", "LPM", "MLD", "CMD", "CMS", "CFS", "GPM", "MGD", "IMGD", "AFD"
}};
inline constexpr std::array<std::string_view, 11> flow_template_units = {{
    "m3/h", "L/s", "L/min", "ML/d", "m3/d", "m3/s", "ft3/s", "US gal/min", "MUSgal/d", "MImpgal/d", "acre.ft/d"
}};

[[nodiscard]] constexpr const UnitField *findUnitField(std::string_view key) noexcept
{
    for (const UnitField &field : unit_fields)
        if (field.key == key) return &field;
    return nullptr;
}

[[nodiscard]] constexpr bool isAllowedUnit(std::string_view quantity, std::string_view unit) noexcept
{
    const UnitField *field = findUnitField(quantity);
    if (field == nullptr || unit.empty()) return false;
    std::string_view remaining = field->choices;
    while (!remaining.empty()) {
        const std::size_t pos = remaining.find(';');
        const std::string_view item = remaining.substr(0, pos);
        if (item == unit) return true;
        if (pos == std::string_view::npos) break;
        remaining.remove_prefix(pos + 1);
    }
    return false;
}

using UnitSelections = std::map<std::string, std::string, std::less<>>;

struct UnitProfile {
    std::string id; // Stable identifier; independent of the user-visible name.
    std::string name;
    UnitSelections overrides; // Missing entries fall back to canonical units.
    bool builtin = false;
};

[[nodiscard]] inline UnitSelections canonicalSelections()
{
    UnitSelections result;
    for (const UnitField &field : unit_fields)
        result.emplace(field.key, canonicalUnit(field.key));
    return result;
}

[[nodiscard]] inline std::string_view resolvedUnit(const UnitProfile &profile, std::string_view quantity)
{
    const auto it = profile.overrides.find(quantity);
    if (it != profile.overrides.end() && isAllowedUnit(quantity, it->second)) return it->second;
    return canonicalUnit(quantity);
}

[[nodiscard]] inline bool setProfileUnit(UnitProfile &profile, std::string_view quantity, std::string_view unit)
{
    if (profile.builtin || !isAllowedUnit(quantity, unit)) return false;
    if (canonicalUnit(quantity) == unit) profile.overrides.erase(std::string(quantity));
    else profile.overrides[std::string(quantity)] = std::string(unit);
    return true;
}

[[nodiscard]] inline UnitProfile canonicalProfile()
{
    return {"canonical", "AOWIS Canonical", {}, true};
}

[[nodiscard]] inline UnitProfile flowTemplateProfile(std::size_t index)
{
    if (index >= flow_template_names.size()) return {};
    UnitProfile result;
    result.id = std::string(flow_template_names[index]);
    result.name = result.id;
    result.builtin = true;
    const auto flow = flow_template_units[index];
    if (flow != canonicalUnit("volumetric_flow_rate"))
        result.overrides.emplace("volumetric_flow_rate", flow);
    if (index >= 6) {
        for (const UnitField &field : unit_fields) {
            if (isAllowedUnit(field.key, "ft") && canonicalUnit(field.key) != "ft")
                result.overrides.emplace(field.key, "ft");
        }
        result.overrides["link_diameter"] = "in";
        result.overrides["pressure"] = "psi";
        result.overrides["power"] = "hp";
    }
    return result;
}

[[nodiscard]] inline std::vector<UnitProfile> predefinedProfiles()
{
    std::vector<UnitProfile> result;
    result.reserve(flow_template_names.size() + 1);
    result.push_back(canonicalProfile());
    for (std::size_t i = 0; i < flow_template_names.size(); ++i)
        result.push_back(flowTemplateProfile(i));
    return result;
}
// Runtime collection is independent of Qt and of any persistence backend.
class UnitProfileCollection {
public:
    UnitProfileCollection() : profiles_(predefinedProfiles()) {}
    [[nodiscard]] const std::vector<UnitProfile> &profiles() const noexcept { return this->profiles_; }
    [[nodiscard]] const std::string &activeId() const noexcept { return this->active_id_; }
    [[nodiscard]] const UnitProfile *find(std::string_view id) const noexcept {
        for (const UnitProfile &profile : this->profiles_)
            if (profile.id == id) return &profile;
        return nullptr;
    }
    [[nodiscard]] bool select(std::string_view id) {
        if (this->find(id) == nullptr) return false;
        this->active_id_ = std::string(id);
        return true;
    }
    [[nodiscard]] bool add(UnitProfile profile) {
        if (profile.builtin || profile.id.empty() || profile.name.empty() || this->find(profile.id) != nullptr)
            return false;
        for (const UnitProfile &existing : this->profiles_)
            if (existing.name == profile.name) return false;
        for (const auto &[key, unit] : profile.overrides)
            if (!isAllowedUnit(key, unit)) return false;
        this->profiles_.push_back(std::move(profile));
        return true;
    }
    [[nodiscard]] bool rename(std::string_view id, std::string name) {
        if (name.empty()) return false;
        for (const UnitProfile &existing : this->profiles_)
            if (existing.name == name && existing.id != id) return false;
        for (UnitProfile &profile : this->profiles_) {
            if (profile.id != id || profile.builtin) continue;
            profile.name = std::move(name);
            return true;
        }
        return false;
    }
    [[nodiscard]] bool remove(std::string_view id) {
        for (auto it = this->profiles_.begin(); it != this->profiles_.end(); ++it) {
            if (it->id != id || it->builtin) continue;
            this->profiles_.erase(it);
            if (this->active_id_ == id) this->active_id_ = "canonical";
            return true;
        }
        return false;
    }
    [[nodiscard]] bool setUnit(std::string_view id, std::string_view quantity, std::string_view unit) {
        for (UnitProfile &profile : this->profiles_)
            if (profile.id == id) return setProfileUnit(profile, quantity, unit);
        return false;
    }
private:
    std::vector<UnitProfile> profiles_;
    std::string active_id_ = "canonical";
};

} // namespace aowis::units

#endif // AOWIS_MODEL_UNITS_UNIT_PROFILE_H
