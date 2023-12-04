#ifndef DESCRIPTORMODELITEM_H
#define DESCRIPTORMODELITEM_H

#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>

#include "../Descriptor/Descriptor.h"


class DescriptorModelItem {
	public:
		explicit DescriptorModelItem(QStringList data, EcuTuner::Descriptor* descriptor = nullptr, DescriptorModelItem *parentItem = nullptr);
		~DescriptorModelItem();

		void appendChild(DescriptorModelItem *child);

		DescriptorModelItem* child(int row);
		int childCount() const;
		int columnCount() const;
		QVariant data(int column) const;
		int row() const;
		DescriptorModelItem* parentItem();

		EcuTuner::Descriptor* GetDescriptor() { return m_descriptorPtr; }
		QStringList Data() { return m_data; }

	private:
		QVector<DescriptorModelItem*> m_childItems;
		DescriptorModelItem* m_parentItem;
		QStringList m_data;

		EcuTuner::Descriptor* m_descriptorPtr;

};


#endif