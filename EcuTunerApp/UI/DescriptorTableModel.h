#ifndef DESCRIPTORTABLEMODEL_H
#define DESCRIPTORTABLEMODEL_H

#include <QAbstractTableModel>

#include "../Ecu/File.h"
#include "../Descriptor/Descriptor.h"

class DescriptorTableModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        explicit DescriptorTableModel(EcuTuner::EcuFile* file, EcuTuner::Descriptor* descriptor, QObject* parent = nullptr);

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;
        
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    signals:
        void editCompleted(const QString&);

    protected:
        EcuTuner::EcuFile* m_ecuFile;
        EcuTuner::Descriptor* m_descriptor;

        double m_maxValue, m_minValue;

};

#endif
