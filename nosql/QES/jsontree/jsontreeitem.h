#ifndef JSON_TREE_ITEM_H
#define JSON_TREE_ITEM_H

#include <QList>
#include <QVector>
#include <QVariant>

class JsonTreeItem
{
	public:
        JsonTreeItem(const QVector<QVariant> &data, JsonTreeItem *parent = 0);
		~JsonTreeItem();


		void appendChild(JsonTreeItem *child);

        bool insertChildren(int position, int count, int columns);
        bool removeChildren(int position, int count);

		JsonTreeItem *child(int row);
		int childCount() const;
		int columnCount() const;

        bool     setData(int column, const QVariant &value);
		QVariant data(int column) const;

		int row() const;
		JsonTreeItem *parent();

	private:
		QList<JsonTreeItem *> childItems;
        QVector<QVariant> itemData;
		JsonTreeItem *parentItem;
};
#endif
