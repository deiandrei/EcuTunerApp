#ifndef DESCRIPTORMODEL_H
#define DESCRIPTORMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qvariant.h>
#include <QList>

#include "../Descriptor/Descriptor.h"
#include "../Ecu/File.h"

class DescriptorModelItem;

//! [0]
class DescriptorModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
        explicit DescriptorModel(QList<EcuTuner::Descriptor*> descriptors, QObject *parent = nullptr);
        ~DescriptorModel();

        QVariant data(const QModelIndex &index, int role) const override;
	    DescriptorModelItem* getItemAtIndex(const QModelIndex& index);
        Qt::ItemFlags flags(const QModelIndex &index) const override;
        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const override;
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex &index) const override;
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	    void doubleClicked(const QModelIndex& index);

        void addItem(EcuTuner::Descriptor* descriptor, DescriptorModelItem* parent);

    private:
        void setupModelData(DescriptorModelItem* parent, QList<EcuTuner::Descriptor*> descriptors);
        DescriptorModelItem* GetFolder(const QString& name);

        DescriptorModelItem* m_rootItem;
};
//! [0]

#endif // DescriptorModel_H