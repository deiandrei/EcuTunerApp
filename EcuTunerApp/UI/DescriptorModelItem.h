#ifndef DESCRIPTORMODELITEM_H
#define DESCRIPTORMODELITEM_H

#include <QtCore/qvariant.h>
#include <QtCore/qvector.h>

#include "../Descriptor/Descriptor.h"

enum FSType { FS_FOLDER, FS_FILE };

class DescriptorModelItem {
	public:
		explicit DescriptorModelItem(EcuTuner::Descriptor* descriptor, DescriptorModelItem *parentItem = nullptr);
		~DescriptorModelItem();

		void appendChild(DescriptorModelItem *child);

		DescriptorModelItem* child(int row);
		int childCount() const;
		int columnCount() const;
		QVariant data(int column) const;
		int row() const;
		DescriptorModelItem* parentItem();

		EcuTuner::Descriptor* GetDescriptor() { return m_descriptorPtr; }

	private:
		QVector<DescriptorModelItem*> m_childItems;
		DescriptorModelItem* m_parentItem;

		EcuTuner::Descriptor* m_descriptorPtr;

};


#endif