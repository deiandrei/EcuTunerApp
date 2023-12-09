#include "DescriptorTableModel.h"

#include <QBrush>
#include "../Common/Utils.h"
#include "../Descriptor/DescriptorReader.h"
#include "../UI/UR/CommandDescriptorValue.h"

DescriptorTableModel::DescriptorTableModel(EcuTuner::EcuFile* file, EcuTuner::Descriptor* descriptor, QObject* parent) : QAbstractTableModel(parent),
    m_ecuFile(file),
    m_descriptor(descriptor)
{
    UpdateMinMax();
}

void DescriptorTableModel::UpdateMinMax() {
    // Calculate max and min
    m_ecuFile->GetBuffer()->Position() = m_descriptor->AxisData->Offset;
    double max = -9999999999.0;
    double min = 9999999999.0;
    double temp = 0.0;
    
    for (int i = 0; i < m_descriptor->AxisData->Rows; ++i) {
        for (int j = 0; j < m_descriptor->AxisData->Columns; ++j) {
            temp = m_descriptor->AxisData->InverseMap ? EcuTuner::DescriptorReader::ReadDataAxisValue(m_ecuFile->GetBuffer(), m_descriptor->AxisData, j, i) : EcuTuner::DescriptorReader::ReadDataAxisValue(m_ecuFile->GetBuffer(), m_descriptor->AxisData, i, j);

            if (temp > max) max = temp;
            if (temp < min) min = temp;
        }
    }
    

    m_maxValue = max;
    m_minValue = min;
}

int DescriptorTableModel::rowCount(const QModelIndex& /*parent*/) const {
    if (!m_descriptor || !m_descriptor->AxisData) return 0;

    return m_descriptor->AxisData->InverseMap ? m_descriptor->AxisData->Columns : m_descriptor->AxisData->Rows;
}

int DescriptorTableModel::columnCount(const QModelIndex& /*parent*/) const {
    if (!m_descriptor || !m_descriptor->AxisData) return 0;

    return m_descriptor->AxisData->InverseMap ? m_descriptor->AxisData->Rows : m_descriptor->AxisData->Columns;
}

QVariant DescriptorTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    ByteBuffer* bb = m_ecuFile->GetBuffer();

    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal && ((m_descriptor->AxisData->InverseMap && m_descriptor->AxisY) || (!m_descriptor->AxisData->InverseMap && m_descriptor->AxisX))) {
            double data = EcuTuner::DescriptorReader::ReadAxisValue(bb, m_descriptor->AxisData->InverseMap ? m_descriptor->AxisY : m_descriptor->AxisX, section);
            int precision = m_descriptor->AxisData->InverseMap ? m_descriptor->AxisY->Precision : m_descriptor->AxisX->Precision;

            return EcuTuner::Utils::FormatPrecision(data, precision);
        }
        else if (orientation == Qt::Vertical && ((m_descriptor->AxisData->InverseMap && m_descriptor->AxisX) || (!m_descriptor->AxisData->InverseMap && m_descriptor->AxisY))) {
            double data = EcuTuner::DescriptorReader::ReadAxisValue(bb, m_descriptor->AxisData->InverseMap ? m_descriptor->AxisX : m_descriptor->AxisY, section);
            int precision = m_descriptor->AxisData->InverseMap ? m_descriptor->AxisX->Precision : m_descriptor->AxisY->Precision;

            return EcuTuner::Utils::FormatPrecision(data, precision);
        }
    }
    else if (role == Qt::SizeHintRole) {
        if (orientation == Qt::Horizontal) return QSize(50, 30);
        else return QSize(75, 35);
    }
    else if (role == Qt::TextAlignmentRole) {
        if (orientation == Qt::Horizontal) return Qt::AlignCenter;
        else return Qt::AlignRight;
    }

    return QVariant();
}

QVariant DescriptorTableModel::data(const QModelIndex& index, int role) const {
    EcuTuner::DataAxis* dataAxis = m_descriptor->AxisData;
    ByteBuffer* bb = m_ecuFile->GetBuffer();

    if (!dataAxis) return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        double data = dataAxis->InverseMap ? EcuTuner::DescriptorReader::ReadDataAxisValue(bb, dataAxis, index.column(), index.row()) : EcuTuner::DescriptorReader::ReadDataAxisValue(bb, dataAxis, index.row(), index.column());
        
        return EcuTuner::Utils::FormatPrecision(data, dataAxis->Precision);
    }
    else if (role == Qt::BackgroundRole) {
        if (m_maxValue > m_minValue) {
            float normalizedDistance = (data(index, Qt::DisplayRole).toString().toDouble() - m_minValue) / (m_maxValue - m_minValue);
            normalizedDistance = std::min(std::max(normalizedDistance, 0.0f), 1.0f);

            return QBrush(QColor::fromRgb((int)std::min(255.0f * 2.0f * normalizedDistance, 255.0f), (int)std::min(255.0f * 2.0f * (1.0f - normalizedDistance), 255.0f), 0));
        }
        else {
            return QBrush(QColor::fromRgb(255, 255, 0));
        }
    }
    else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    return QVariant();
}

bool DescriptorTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    EcuTuner::DataAxis* dataAxis = m_descriptor->AxisData;
    ByteBuffer* bb = m_ecuFile->GetBuffer();
    double oldValue = data(index, role).toDouble();

    if (!dataAxis) return false;

    if (role == Qt::EditRole) {
        if (!checkIndex(index) || value == oldValue) return false;
        
        bool conversionStatus = 0;
        double valueFromUI = value.toString().toDouble(&conversionStatus);
        if (!conversionStatus) return false;

        //URManager::Instance()->PushCommand(new CommandDescriptorValue(m_ecuFile, m_descriptor, index.row(), index.column(), valueFromUI, oldValue));

        if(dataAxis->InverseMap) EcuTuner::DescriptorReader::SetDataAxisValue(bb, dataAxis, index.column(), index.row(), valueFromUI);
        else EcuTuner::DescriptorReader::SetDataAxisValue(bb, dataAxis, index.row(), index.column(), valueFromUI);
        
        UpdateMinMax();
        
        emit dataChanged(this->index(0,0), this->index(rowCount(), columnCount()));

        return true;
    }

    return false;
}

void DescriptorTableModel::incrementIndex(const QModelIndex& index, double factor) {
    if (!checkIndex(index)) return;

    double step = m_descriptor->AxisData->ValueFactor;

    setData(index, data(index).toDouble() + factor * step);
}

Qt::ItemFlags DescriptorTableModel::flags(const QModelIndex& index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}
