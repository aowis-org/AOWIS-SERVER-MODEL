#ifndef AOWIS_MODEL_MEASUREMENT_WATER_METER_H
#define AOWIS_MODEL_MEASUREMENT_WATER_METER_H

#include <QString>
#include <QUuid>

#include "../entity.h"

// Physical water-meter identity and asset information. Meter observations and
// time series belong to the measurement-data model rather than this device record.
struct WaterMeter
{
    QString id;
    QUuid uuid;

    QString manufacturer;
    QString model;
    QString serial_number;

    EntityMetadata metadata;
};

#endif // AOWIS_MODEL_MEASUREMENT_WATER_METER_H
