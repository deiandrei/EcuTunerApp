#include "DescriptorTableModel.h"

#include <QBrush>

DescriptorTableModel::DescriptorTableModel(EcuTuner::EcuFile* file, EcuTuner::Descriptor* descriptor, QObject* parent) : QAbstractTableModel(parent),
    m_ecuFile(file),
    m_descriptor(descriptor)
{
    file->GetBuffer()->Position() = descriptor->AxisData->Offset;
    uint2 max = std::numeric_limits<uint2>::min();
    uint2 min = std::numeric_limits<uint2>::max();
    uint2 temp;
    for (int i = 0; i < descriptor->AxisData->Rows; ++i) {
        for (int j = 0; j < descriptor->AxisData->Columns; ++j) {
            temp = file->GetBuffer()->Read<uint2>();

            if (temp > max) max = temp;
            if (temp < min) min = temp;
        }
    }

    m_maxValue = descriptor->AxisData->Parse(max);
    m_minValue = descriptor->AxisData->Parse(min);
}

int DescriptorTableModel::rowCount(const QModelIndex& /*parent*/) const {
    if (!m_descriptor || !m_descriptor->AxisData) return 0;

    return m_descriptor->AxisData->Rows;
}

int DescriptorTableModel::columnCount(const QModelIndex& /*parent*/) const {
    if (!m_descriptor || !m_descriptor->AxisData) return 0;

    return m_descriptor->AxisData->Columns;
}

QVariant DescriptorTableModel::data(const QModelIndex& index, int role) const
{
    EcuTuner::DataAxis* dataAxis = m_descriptor->AxisData;
    ByteBuffer* bb = m_ecuFile->GetBuffer();

    if (!dataAxis) return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        int blockSize = 2;
        int offset = m_descriptor->InverseMap ? (dataAxis->Rows * index.column() + index.row()) : (dataAxis->Columns * index.row() + index.column());
        int dataPos = dataAxis->Offset + offset * blockSize;
        
        bb->Position() = dataPos;
        uint2 data = bb->Read<uint2>();

        return QString::number(dataAxis->Parse(data));
    }
    else if (role == Qt::BackgroundRole) {
        float normalizedDistance = (data(index, Qt::DisplayRole).toString().toDouble() - m_minValue) / (m_maxValue - m_minValue);
        normalizedDistance = std::min(std::max(normalizedDistance, 0.0f), 1.0f);

		return QBrush(QColor::fromRgb((int)std::min(255.0f * 2.0f * normalizedDistance, 255.0f), (int)std::min(255.0f * 2.0f * (1.0f - normalizedDistance), 255.0f), 0));
    }

    return QVariant();
}

bool DescriptorTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    EcuTuner::DataAxis* dataAxis = m_descriptor->AxisData;
    ByteBuffer* bb = m_ecuFile->GetBuffer();

    if (!dataAxis) return false;

    if (role == Qt::EditRole) {
        if (!checkIndex(index) || value == data(index, role)) return false;
        
        int blockSize = 2;
        int offset = m_descriptor->InverseMap ? (dataAxis->Rows * index.column() + index.row()) : (dataAxis->Columns * index.row() + index.column());
        int dataPos = dataAxis->Offset + offset * blockSize;

        bool conversionStatus = 0;
        double valueFromUI = value.toString().toDouble(&conversionStatus);
        if (!conversionStatus) return false;

        uint2 valueToSet = dataAxis->UnParse(valueFromUI);

        m_maxValue = std::max(m_maxValue, valueFromUI);
        m_minValue = std::min(m_minValue, valueFromUI);

        memcpy(m_ecuFile->GetBuffer()->Data() + dataPos, &valueToSet, blockSize);

        return true;
    }
    return false;
}

Qt::ItemFlags DescriptorTableModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}
