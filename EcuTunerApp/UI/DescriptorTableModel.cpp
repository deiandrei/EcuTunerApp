#include "DescriptorTableModel.h"

#include <QBrush>
#include "../Descriptor/DescriptorReader.h"

DescriptorTableModel::DescriptorTableModel(EcuTuner::EcuFile* file, EcuTuner::Descriptor* descriptor, QObject* parent) : QAbstractTableModel(parent),
    m_ecuFile(file),
    m_descriptor(descriptor)
{
    UpdateMinMax();
}

void DescriptorTableModel::UpdateMinMax() {
    // Calculate max and min
    m_ecuFile->GetBuffer()->Position() = m_descriptor->AxisData->Offset;
    uint2 max = std::numeric_limits<uint2>::min();
    uint2 min = std::numeric_limits<uint2>::max();
    uint2 temp;
    for (int i = 0; i < m_descriptor->AxisData->Rows; ++i) {
        for (int j = 0; j < m_descriptor->AxisData->Columns; ++j) {
            temp = m_ecuFile->GetBuffer()->Read<uint2>();

            if (temp > max) max = temp;
            if (temp < min) min = temp;
        }
    }

    m_maxValue = m_descriptor->AxisData->Parse(max);
    m_minValue = m_descriptor->AxisData->Parse(min);
}

int DescriptorTableModel::rowCount(const QModelIndex& /*parent*/) const {
    if (!m_descriptor || !m_descriptor->AxisData) return 0;

    return m_descriptor->AxisData->Rows;
}

int DescriptorTableModel::columnCount(const QModelIndex& /*parent*/) const {
    if (!m_descriptor || !m_descriptor->AxisData) return 0;

    return m_descriptor->AxisData->Columns;
}

QVariant DescriptorTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    ByteBuffer* bb = m_ecuFile->GetBuffer();

    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal && m_descriptor->AxisX) {
            double data = EcuTuner::DescriptorReader::ReadAxisValue(bb, m_descriptor->AxisX, section);

            return QString::number(data);
        }
        else if (orientation == Qt::Vertical && m_descriptor->AxisY) {
            double data = EcuTuner::DescriptorReader::ReadAxisValue(bb, m_descriptor->AxisY, section);

            return QString::number(data);
        }
    }

    return QVariant();
}

QVariant DescriptorTableModel::data(const QModelIndex& index, int role) const {
    EcuTuner::DataAxis* dataAxis = m_descriptor->AxisData;
    ByteBuffer* bb = m_ecuFile->GetBuffer();

    if (!dataAxis) return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        double data = EcuTuner::DescriptorReader::ReadDataAxisValue(bb, dataAxis, index.row(), index.column());

        return QString::number(data);
    }
    else if (role == Qt::BackgroundRole) {
        float normalizedDistance = (data(index, Qt::DisplayRole).toString().toDouble() - m_minValue) / (m_maxValue - m_minValue);
        normalizedDistance = std::min(std::max(normalizedDistance, 0.0f), 1.0f);

		return QBrush(QColor::fromRgb((int)std::min(255.0f * 2.0f * normalizedDistance, 255.0f), (int)std::min(255.0f * 2.0f * (1.0f - normalizedDistance), 255.0f), 0));
    }

    return QVariant();
}

bool DescriptorTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    EcuTuner::DataAxis* dataAxis = m_descriptor->AxisData;
    ByteBuffer* bb = m_ecuFile->GetBuffer();

    if (!dataAxis) return false;

    if (role == Qt::EditRole) {
        if (!checkIndex(index) || value == data(index, role)) return false;
        
        bool conversionStatus = 0;
        double valueFromUI = value.toString().toDouble(&conversionStatus);
        if (!conversionStatus) return false;

        m_maxValue = std::max(m_maxValue, valueFromUI);
        m_minValue = std::min(m_minValue, valueFromUI);

        EcuTuner::DescriptorReader::SetDataAxisValue(bb, dataAxis, index.row(), index.column(), valueFromUI);
        
        return true;
    }

    return false;
}

Qt::ItemFlags DescriptorTableModel::flags(const QModelIndex& index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}
