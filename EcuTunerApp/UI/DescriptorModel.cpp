#include "DescriptorModel.h"
#include "DescriptorModelItem.h"

DescriptorModel::DescriptorModel(QList<EcuTuner::Descriptor*> descriptors, QObject* parent) : QAbstractItemModel(parent) {
    rootItem = new DescriptorModelItem(nullptr);
    setupModelData(rootItem, descriptors);
}

DescriptorModel::~DescriptorModel()
{
    delete rootItem;
}

int DescriptorModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<DescriptorModelItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

void DescriptorModel::doubleClicked(const QModelIndex& index) {
	int x = 0;
}

QVariant DescriptorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    DescriptorModelItem *item = static_cast<DescriptorModelItem*>(index.internalPointer());

    return item->data(index.column());
}

DescriptorModelItem* DescriptorModel::getItemAtIndex(const QModelIndex& index) {
	if (!index.isValid())
		return nullptr;

	DescriptorModelItem* item = static_cast<DescriptorModelItem*>(index.internalPointer());

	return item;
}

Qt::ItemFlags DescriptorModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant DescriptorModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex DescriptorModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DescriptorModelItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DescriptorModelItem*>(parent.internalPointer());

    DescriptorModelItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex DescriptorModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    DescriptorModelItem *childItem = static_cast<DescriptorModelItem*>(index.internalPointer());
    DescriptorModelItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int DescriptorModel::rowCount(const QModelIndex &parent) const
{
    DescriptorModelItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DescriptorModelItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void DescriptorModel::setupModelData(DescriptorModelItem* parent, QList<EcuTuner::Descriptor*> descriptors) {
    for (auto* descriptor : descriptors) {
		DescriptorModelItem* item = new DescriptorModelItem(descriptor, parent);
		parent->appendChild(item);
	}
}

void DescriptorModel::addItem(EcuTuner::Descriptor* descriptor) {
    DescriptorModelItem* item = new DescriptorModelItem(descriptor, rootItem);
    rootItem->appendChild(item);
}
