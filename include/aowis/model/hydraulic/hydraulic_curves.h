#ifndef AOWIS_MODEL_HYDRAULIC_CURVES_H
#define AOWIS_MODEL_HYDRAULIC_CURVES_H

#include <QList>
#include <QString>
#include <QUuid>

struct HydraulicPatternTime
{
    QString id;
    QUuid uuid;
    QList<double> factors;
    QString comment;
};

struct HydraulicCurveTankVolumePoint
{
    double water_level_m = 0.0;
    double volume_m3 = 0.0;
};

struct HydraulicCurveTankVolume
{
    QString id;
    QUuid uuid;
    QList<HydraulicCurveTankVolumePoint> points;
    QString comment;
};

struct HydraulicCurvePumpHeadPoint
{
    double flow_m3_per_h = 0.0;
    double head_gain_m = 0.0;
};

struct HydraulicCurvePumpHead
{
    QString id;
    QUuid uuid;
    QList<HydraulicCurvePumpHeadPoint> points;
    QString comment;
};

struct HydraulicCurvePumpEfficiencyPoint
{
    double flow_m3_per_h = 0.0;
    double efficiency_percent = 0.0;
};

struct HydraulicCurvePumpEfficiency
{
    QString id;
    QUuid uuid;
    QList<HydraulicCurvePumpEfficiencyPoint> points;
    QString comment;
};

struct HydraulicCurveValveHeadlossPoint
{
    double flow_m3_per_h = 0.0;
    double head_loss_m = 0.0;
};

struct HydraulicCurveValveHeadloss
{
    QString id;
    QUuid uuid;
    QList<HydraulicCurveValveHeadlossPoint> points;
    QString comment;
};

struct HydraulicCurveValveCharacteristicPoint
{
    double position_percent = 0.0;
    double relative_flow_percent = 0.0;
};

struct HydraulicCurveValveCharacteristic
{
    QString id;
    QUuid uuid;
    QList<HydraulicCurveValveCharacteristicPoint> points;
    QString comment;
};

struct HydraulicCurveGenericPoint
{
    double x = 0.0;
    double y = 0.0;
};

struct HydraulicCurveGeneric
{
    QString id;
    QUuid uuid;
    QList<HydraulicCurveGenericPoint> points;
    QString comment;
};

#endif // AOWIS_MODEL_HYDRAULIC_CURVES_H
