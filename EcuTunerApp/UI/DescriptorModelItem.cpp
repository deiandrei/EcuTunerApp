#include "DescriptorModelItem.h"

DescriptorModelItem::DescriptorModelItem(EcuTuner::Descriptor* descriptor, DescriptorModelItem *parent) : m_parentItem(parent) {
    m_descriptorPtr = descriptor;
}

DescriptorModelItem::~DescriptorModelItem()
{
    qDeleteAll(m_childItems);
}

void DescriptorModelItem::appendChild(DescriptorModelItem *item)
{
    m_childItems.append(item);
}

DescriptorModelItem *DescriptorModelItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int DescriptorModelItem::childCount() const {
    return m_childItems.count();
}

int DescriptorModelItem::columnCount() const {
    return 1;
}

QVariant DescriptorModelItem::data(int column) const {
    if(m_descriptorPtr) return (m_descriptorPtr->Alias + " " + m_descriptorPtr->Name);

    return "Root";
}

DescriptorModelItem *DescriptorModelItem::parentItem() {
    return m_parentItem;
}

int DescriptorModelItem::row() const {
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<DescriptorModelItem*>(this));

    return 0;
}
