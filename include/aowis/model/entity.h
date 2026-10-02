#ifndef AOWIS_MODEL_ENTITY_H
#define AOWIS_MODEL_ENTITY_H

#include <optional>

#include <QDate>
#include <QString>
#include <QStringList>

enum class EntityModelRole
{
    Unspecified,
    ExistingAsset,
    PlannedAsset,
    VirtualModelElement,
    BoundaryCondition,
    TemporaryTesting,
    RetiredAsset
};

struct EntityMetadata
{
    bool enabled = true;
    EntityModelRole model_role = EntityModelRole::Unspecified;
    std::optional<QDate> date_added;
    std::optional<QDate> date_installed;

    // Stable human-readable explanation of the entity.
    QString description;
    // Free-form note that can change independently of the description.
    QString comment;
    QStringList tags;
};

#endif // AOWIS_MODEL_ENTITY_H
