#ifndef AOWIS_MODEL_HYDRAULIC_NETWORK_PRESENTATION_UNITS_H
#define AOWIS_MODEL_HYDRAULIC_NETWORK_PRESENTATION_UNITS_H

#include <QList>
#include <QString>

// Presentation units stored with a network ("project units"). Purely a
// display choice: every measurement in the Model stays in its canonical unit
// (encoded in the field name), so these never change a hydraulic value.
//
// A stored unit set is a self-contained copy, never a reference to a unit
// profile: built-in profiles may change between releases and custom profiles
// live only in one user's local configuration.

// One display choice: quantity is an aowis::units::unit_fields key (for
// example "volumetric_flow_rate"), unit one of that quantity's choices.
struct NetworkPresentationUnit
{
    QString quantity;
    QString unit;
};

struct NetworkPresentationUnits
{
    // User-visible name of the unit set (for example "LPS" or "Utility SI").
    QString name;
    // Informational: id of the unit profile it was copied from (a built-in id
    // such as "GPM", or a custom profile id); empty if unknown. Never resolved.
    QString derived_from_profile;
    // Canonical form: sorted by quantity, each quantity at most once. A
    // quantity absent here falls back to the viewer's local choice.
    QList<NetworkPresentationUnit> units;
};

#endif // AOWIS_MODEL_HYDRAULIC_NETWORK_PRESENTATION_UNITS_H
