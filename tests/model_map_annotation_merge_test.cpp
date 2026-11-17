#include <aowis/model/edit/model_edit_transaction.h>
#include <aowis/model/map/map_annotation.h>
#include <aowis/model/serialization/model_codec.h>
#include <aowis/model/serialization/model_field_merge.h>
#include <aowis/model/uuid.h>

#include <QByteArray>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QUuid>

#include <cstdlib>
#include <iostream>
#include <optional>

namespace
{
void require(bool condition, const char *description)
{
    if (!condition)
    {
        std::cerr << "Map annotation merge/transaction: " << description << '\n';
        std::exit(EXIT_FAILURE);
    }
}

MapAnnotation sample()
{
    MapAnnotation annotation;
    annotation.uuid = createUuidV7();
    annotation.text = QStringLiteral("Service access at the pump station");
    annotation.position_wgs84.longitude_deg = 13.4;
    annotation.position_wgs84.latitude_deg = 52.5;
    annotation.position_wgs84.altitude_m = 22.0;
    return annotation;
}

std::optional<aowis::model::ModelFieldMerge> mergeAnnotation(
    const MapAnnotation &baseline, const MapAnnotation &left, const MapAnnotation &right,
    const QMap<QString, bool> &choices = {})
{
    using aowis::model::ModelCodec;
    return aowis::model::mergeModelFields(baseline.uuid, QStringLiteral("map.annotation"),
        ModelCodec<MapAnnotation>::encode(baseline), ModelCodec<MapAnnotation>::encode(left),
        ModelCodec<MapAnnotation>::encode(right), choices);
}
} // namespace

int main()
{
    using aowis::model::ModelCodec;
    using aowis::model::ModelFieldMerge;

    // Annotation edits remain ordinary reversible MODEL transaction images.
    const MapAnnotation before = sample();
    MapAnnotation after = before;
    after.text = QStringLiteral("Updated service access");
    aowis::model::ModelEditTransaction transaction;
    transaction.transactionId = createUuidV7();
    transaction.operation = QStringLiteral("map.annotation.update");
    transaction.edits.append(aowis::model::ModelEntityEdit{
        after.uuid, QStringLiteral("MapAnnotation"),
        ModelCodec<MapAnnotation>::encode(before), ModelCodec<MapAnnotation>::encode(after)});
    const QByteArray transactionBytes = aowis::model::ModelEditTransactionCodec::encode(transaction);
    const std::optional<aowis::model::ModelEditTransaction> decoded =
        aowis::model::ModelEditTransactionCodec::decode(transactionBytes);
    require(!transactionBytes.isEmpty() && decoded &&
        aowis::model::ModelEditTransactionCodec::encode(*decoded) == transactionBytes &&
        decoded->edits.size() == 1, "canonical reversible annotation transaction");

    // Geometry-free anchor edits remain a single MODEL field for merge purposes.
    MapAnnotationAnchor anchor;
    anchor.entity_uuid = createUuidV7();
    anchor.entity_type = QStringLiteral("HydraulicNodeTank");

    // Merge disjoint text and appearance changes without an entity-level conflict.
    const MapAnnotation baseline = sample();
    MapAnnotation left = baseline;
    MapAnnotation right = baseline;
    left.text = QStringLiteral("Left text");
    left.style.show_background = true;
    right.style.text_size = MapAnnotationTextSize::Large;
    right.style.visible = false;
    const std::optional<ModelFieldMerge> independent = mergeAnnotation(baseline, left, right);
    const std::optional<MapAnnotation> independentValue = independent && !independent->content.isEmpty() ?
        ModelCodec<MapAnnotation>::decode(independent->content) : std::nullopt;
    require(independent && independent->conflicting_fields.isEmpty() && independentValue &&
        independentValue->text == left.text && independentValue->style.show_background &&
        independentValue->style.text_size == right.style.text_size &&
        !independentValue->style.visible, "disjoint annotation edits merge by field");
    left = baseline;
    right = baseline;
    left.style.text_size = MapAnnotationTextSize::Small;
    right.style.text_size = MapAnnotationTextSize::Large;
    const std::optional<ModelFieldMerge> styleConflict = mergeAnnotation(baseline, left, right);
    require(styleConflict && styleConflict->content.isEmpty() &&
        styleConflict->conflicting_fields == QStringList{QStringLiteral("style.text_size")},
        "conflicting appearance edits report precise field name");
    const std::optional<ModelFieldMerge> chosen = mergeAnnotation(baseline, left, right,
        {{QStringLiteral("style.text_size"), true}});
    const std::optional<MapAnnotation> chosenValue = chosen ?
        ModelCodec<MapAnnotation>::decode(chosen->content) : std::nullopt;
    require(chosenValue && chosenValue->style.text_size == MapAnnotationTextSize::Large,
        "explicit field choice resolves conflict");

    // Attachment target and offsets form one atomic semantic merge field.
    left = baseline;
    right = baseline;
    left.anchor = anchor;
    right.text = QStringLiteral("Edited separately");
    const std::optional<ModelFieldMerge> anchorText = mergeAnnotation(baseline, left, right);
    const std::optional<MapAnnotation> anchorTextValue = anchorText ?
        ModelCodec<MapAnnotation>::decode(anchorText->content) : std::nullopt;
    require(anchorTextValue && anchorTextValue->anchor &&
        anchorTextValue->text == right.text, "anchor and text merge independently");
    right = left;
    right.anchor->offset_north_m += 2.0;
    left.anchor->offset_east_m += 3.0;
    const std::optional<ModelFieldMerge> anchorConflict = mergeAnnotation(baseline, left, right);
    require(anchorConflict && anchorConflict->conflicting_fields == QStringList{QStringLiteral("anchor")},
        "conflicting anchors are not merged component-by-component");

    // UUID/domain identity must be immutable for every branch.
    left = baseline;
    left.uuid = createUuidV7();
    require(!mergeAnnotation(baseline, left, baseline), "annotation UUID changes rejected");
    MapAnnotationLayer layer;
    layer.uuid = createUuidV7();
    layer.domain = MapAnnotationDomain::Distribution;
    layer.owner_network_uuid = createUuidV7();
    MapAnnotationLayer layerLeft = layer;
    MapAnnotationLayer layerRight = layer;
    layerLeft.owner_network_uuid = createUuidV7();
    layerRight.owner_network_uuid = createUuidV7();
    const QString layerKind = QStringLiteral("map.annotation_layer");
    const QByteArray layerBase = ModelCodec<MapAnnotationLayer>::encode(layer);
    const std::optional<ModelFieldMerge> layerConflict = aowis::model::mergeModelFields(
        layer.uuid, layerKind, layerBase,
        ModelCodec<MapAnnotationLayer>::encode(layerLeft), ModelCodec<MapAnnotationLayer>::encode(layerRight));
    require(layerConflict && layerConflict->conflicting_fields == QStringList{QStringLiteral("owner_network_uuid")},
        "concurrent layer ownership changes conflict");
    const std::optional<ModelFieldMerge> layerResolved = aowis::model::mergeModelFields(
        layer.uuid, layerKind, layerBase,
        ModelCodec<MapAnnotationLayer>::encode(layerLeft), ModelCodec<MapAnnotationLayer>::encode(layerRight),
        {{QStringLiteral("owner_network_uuid"), false}});
    const std::optional<MapAnnotationLayer> layerValue = layerResolved ?
        ModelCodec<MapAnnotationLayer>::decode(layerResolved->content) : std::nullopt;
    require(layerValue && layerValue->owner_network_uuid == layerLeft.owner_network_uuid,
        "explicit ownership conflict resolution");
    layerRight.domain = MapAnnotationDomain::Drainage;
    layerRight.owner_network_uuid.reset();
    require(!aowis::model::mergeModelFields(layer.uuid, layerKind, layerBase,
        ModelCodec<MapAnnotationLayer>::encode(layerLeft), ModelCodec<MapAnnotationLayer>::encode(layerRight)),
        "domain mutation rejected rather than merged");

    return EXIT_SUCCESS;
}
