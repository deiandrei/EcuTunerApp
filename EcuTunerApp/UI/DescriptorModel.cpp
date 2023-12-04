#include "DescriptorModel.h"
#include "DescriptorModelItem.h"

DescriptorModel::DescriptorModel(QList<EcuTuner::Descriptor*> descriptors, QObject* parent) : QAbstractItemModel(parent) {
    QStringList headerData = {"Name", "Alias"};
    m_rootItem = new DescriptorModelItem(headerData);

    setupModelData(m_rootItem, descriptors);
}

DescriptorModel::~DescriptorModel()
{
    delete m_rootItem;
}

int DescriptorModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<DescriptorModelItem*>(parent.internalPointer())->columnCount();
    return m_rootItem->columnCount();
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
        return m_rootItem->data(section);

    return QVariant();
}

QModelIndex DescriptorModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DescriptorModelItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<DescriptorModelItem*>(parent.internalPointer());

    DescriptorModelItem *childItem = parentItem->child(row);
    if (childItem) return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex DescriptorModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    DescriptorModelItem *childItem = static_cast<DescriptorModelItem*>(index.internalPointer());
    DescriptorModelItem *parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int DescriptorModel::rowCount(const QModelIndex &parent) const
{
    DescriptorModelItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<DescriptorModelItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void DescriptorModel::setupModelData(DescriptorModelItem* parent, QList<EcuTuner::Descriptor*> descriptors) {
    for (auto* descriptor : descriptors) {
        QString folderName = descriptor->Folder;
        if (folderName.isEmpty()) folderName = "Uncategorized";

        DescriptorModelItem* folder = GetFolder(folderName);

        addItem(descriptor, folder);
	}
}

DescriptorModelItem* DescriptorModel::GetFolder(const QString& name) {
    for (int i = 0; i < m_rootItem->childCount(); ++i) {
        if (m_rootItem->child(i)->data(0) == name) return m_rootItem->child(i);
    }

    DescriptorModelItem* newItem = new DescriptorModelItem({ name, "" }, nullptr, m_rootItem);
    m_rootItem->appendChild(newItem);
    return newItem;
}

void DescriptorModel::addItem(EcuTuner::Descriptor* descriptor, DescriptorModelItem* parent) {
    QStringList itemData = { descriptor->Name, descriptor->Alias };
    DescriptorModelItem* item = new DescriptorModelItem(itemData, descriptor, parent);
    parent->appendChild(item);
}
