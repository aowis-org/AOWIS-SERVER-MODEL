#ifndef AOWIS_MODEL_HYDRAULIC_PIPE_MATERIALS_H
#define AOWIS_MODEL_HYDRAULIC_PIPE_MATERIALS_H

#include <optional>

#include <QDate>
#include <QList>
#include <QString>
#include <QUuid>

#include "hydraulic_types.h"

enum class HydraulicPipeRoughnessMode
{
    Explicit,
    MaterialLibrary
};

struct HydraulicPipeMaterialRoughnessAtAge
{
    int age_years = 0;
    std::optional<double> roughness_hazen_williams;
    std::optional<double> roughness_darcy_weisbach_mm;
    std::optional<double> roughness_chezy_manning;
};

struct HydraulicPipeMaterial
{
    QString id;
    QUuid uuid;
    QString description;
    QList<HydraulicPipeMaterialRoughnessAtAge> roughness_by_age;
};

std::optional<int> hydraulicPipeAgeYears(
    const QDate &date_installed, const QDate &reference_date);

std::optional<double> resolveHydraulicPipeMaterialRoughness(
    const HydraulicPipeMaterial &material,
    HydraulicHeadlossFormula headloss_formula,
    int age_years);

#endif // AOWIS_MODEL_HYDRAULIC_PIPE_MATERIALS_H
