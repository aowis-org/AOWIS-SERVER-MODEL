#include <aowis/model/hydraulic/hydraulic_pipe_materials.h>

#include <cmath>
#include <limits>

std::optional<int> hydraulicPipeAgeYears(
    const QDate &date_installed, const QDate &reference_date)
{
    if (!date_installed.isValid() || !reference_date.isValid())
        return std::nullopt;

    const int age_years = reference_date.year() - date_installed.year();
    if (age_years < 0)
        return std::nullopt;

    return age_years;
}

std::optional<double> resolveHydraulicPipeMaterialRoughness(
    const HydraulicPipeMaterial &material,
    HydraulicHeadlossFormula headloss_formula,
    int age_years)
{
    if (age_years < 0)
        return std::nullopt;

    const HydraulicPipeMaterialRoughnessAtAge *selected_entry = nullptr;
    int selected_age = std::numeric_limits<int>::min();

    for (const HydraulicPipeMaterialRoughnessAtAge &entry : material.roughness_by_age)
    {
        if (entry.age_years < 0 || entry.age_years > age_years)
            continue;

        std::optional<double> roughness;
        switch (headloss_formula)
        {
        case HydraulicHeadlossFormula::HazenWilliams:
            roughness = entry.roughness_hazen_williams;
            break;
        case HydraulicHeadlossFormula::DarcyWeisbach:
            roughness = entry.roughness_darcy_weisbach_mm;
            break;
        case HydraulicHeadlossFormula::ChezyManning:
            roughness = entry.roughness_chezy_manning;
            break;
        }

        if (!roughness.has_value())
            continue;

        if (selected_entry == nullptr || entry.age_years > selected_age)
        {
            selected_entry = &entry;
            selected_age = entry.age_years;
        }
    }

    if (selected_entry == nullptr)
        return std::nullopt;

    switch (headloss_formula)
    {
    case HydraulicHeadlossFormula::HazenWilliams:
        return selected_entry->roughness_hazen_williams;
    case HydraulicHeadlossFormula::DarcyWeisbach:
        return selected_entry->roughness_darcy_weisbach_mm;
    case HydraulicHeadlossFormula::ChezyManning:
        return selected_entry->roughness_chezy_manning;
    }

    return std::nullopt;
}
