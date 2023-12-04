#include "DescriptorModelItem.h"

DescriptorModelItem::DescriptorModelItem(QStringList data, EcuTuner::Descriptor* descriptor, DescriptorModelItem* parentItem) : m_parentItem(parentItem) {
    m_descriptorPtr = descriptor;
    m_data = data;
}

DescriptorModelItem::~DescriptorModelItem()
{
    qDeleteAll(m_childItems);
}

void DescriptorModelItem::appendChild(DescriptorModelItem* item)
{
    m_childItems.append(item);
}

DescriptorModelItem* DescriptorModelItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size()) return nullptr;

    return m_childItems.at(row);
}

int DescriptorModelItem::childCount() const {
    return m_childItems.count();
}

int DescriptorModelItem::columnCount() const {
    return m_data.size();
}

QVariant DescriptorModelItem::data(int column) const {
    if (column >= m_data.size()) return "";

    return m_data[column];
}

DescriptorModelItem* DescriptorModelItem::parentItem() {
    return m_parentItem;
}

int DescriptorModelItem::row() const {
    if (m_parentItem) return m_parentItem->m_childItems.indexOf(const_cast<DescriptorModelItem*>(this));

    return 0;
}
