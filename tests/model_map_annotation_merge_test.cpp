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
        after.uuid, QStringLiteral("map.annotation"),
        ModelCodec<MapAnnotation>::encode(before), ModelCodec<MapAnnotation>::encode(after), createUuidV7()});
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

    // Attachment target, offsets and the explicit position form one atomic
    // semantic merge field ("placement").
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
    require(anchorConflict && anchorConflict->content.isEmpty() &&
        anchorConflict->conflicting_fields == QStringList{QStringLiteral("placement")},
        "conflicting anchors are not merged component-by-component");
    const std::optional<ModelFieldMerge> anchorChosen = mergeAnnotation(baseline, left, right,
        {{QStringLiteral("placement"), true}});
    const std::optional<MapAnnotation> anchorChosenValue = anchorChosen ?
        ModelCodec<MapAnnotation>::decode(anchorChosen->content) : std::nullopt;
    require(anchorChosenValue && anchorChosenValue->anchor &&
        anchorChosenValue->anchor->offset_north_m == right.anchor->offset_north_m &&
        anchorChosenValue->anchor->offset_east_m == right.anchor->offset_east_m,
        "a placement choice takes the whole anchor of the chosen side");

    // A10: anchor and position are ONE placement. Attaching keeps the last
    // explicit position, so a concurrent free drag must not be combined.
    left = baseline;
    right = baseline;
    left.anchor = anchor;
    right.position_wgs84.longitude_deg += 0.01;
    right.text = QStringLiteral("Dragged and edited");
    const std::optional<ModelFieldMerge> attachDrag = mergeAnnotation(baseline, left, right);
    require(attachDrag && attachDrag->content.isEmpty() &&
        attachDrag->conflicting_fields == QStringList{QStringLiteral("placement")},
        "attach versus free drag conflicts as one placement field");
    const std::optional<ModelFieldMerge> keepDrag = mergeAnnotation(baseline, left, right,
        {{QStringLiteral("placement"), true}});
    const std::optional<MapAnnotation> keepDragValue = keepDrag ?
        ModelCodec<MapAnnotation>::decode(keepDrag->content) : std::nullopt;
    require(keepDragValue && !keepDragValue->anchor &&
        keepDragValue->position_wgs84.longitude_deg == right.position_wgs84.longitude_deg &&
        keepDragValue->text == right.text,
        "choosing the dragged placement drops the attach but keeps the text edit");
    const std::optional<ModelFieldMerge> keepAttach = mergeAnnotation(baseline, left, right,
        {{QStringLiteral("placement"), false}});
    const std::optional<MapAnnotation> keepAttachValue = keepAttach ?
        ModelCodec<MapAnnotation>::decode(keepAttach->content) : std::nullopt;
    require(keepAttachValue && keepAttachValue->anchor &&
        keepAttachValue->position_wgs84.longitude_deg == baseline.position_wgs84.longitude_deg &&
        keepAttachValue->text == right.text,
        "choosing the attach keeps its explicit position, never the other drag");

    // Two different drags conflict as placement; one drag merges with text.
    left = baseline;
    right = baseline;
    left.position_wgs84.latitude_deg += 0.01;
    right.position_wgs84.latitude_deg -= 0.01;
    const std::optional<ModelFieldMerge> dragDrag = mergeAnnotation(baseline, left, right);
    require(dragDrag && dragDrag->conflicting_fields == QStringList{QStringLiteral("placement")},
        "two different drags conflict as placement");
    right = baseline;
    right.text = QStringLiteral("Only text");
    const std::optional<ModelFieldMerge> dragText = mergeAnnotation(baseline, left, right);
    const std::optional<MapAnnotation> dragTextValue = dragText ?
        ModelCodec<MapAnnotation>::decode(dragText->content) : std::nullopt;
    require(dragTextValue && dragTextValue->position_wgs84.latitude_deg == left.position_wgs84.latitude_deg &&
        dragTextValue->text == right.text, "a one-sided drag merges with a text edit");
    right = left;
    const std::optional<ModelFieldMerge> sameDrag = mergeAnnotation(baseline, left, right);
    require(sameDrag && sameDrag->conflicting_fields.isEmpty(), "identical placements do not conflict");

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
