/*
    JSON Tree Item

    A container for items of data supplied by the JSON Tree Model.
*/

#include <QStringList>

#include "jsontreeitem.h"

JsonTreeItem::JsonTreeItem(const QVector<QVariant> &data, JsonTreeItem *parent)
{
	parentItem = parent;
	itemData = data;
}

JsonTreeItem::~JsonTreeItem()
{
	qDeleteAll(childItems);
}

void JsonTreeItem::appendChild(JsonTreeItem *item)
{
	childItems.append(item);
}

JsonTreeItem *JsonTreeItem::child(int row)
{
	return childItems.value(row);
}

int JsonTreeItem::childCount() const
{
	return childItems.count();
}

int JsonTreeItem::columnCount() const
{
	return itemData.count();
}

QVariant JsonTreeItem::data(int column) const
{
	return itemData.value(column);
}

JsonTreeItem *JsonTreeItem::parent()
{
	return parentItem;
}

int JsonTreeItem::row() const
{
	if (parentItem) {
		return parentItem->childItems.indexOf(const_cast<JsonTreeItem *>(this));
	}

	return 0;
}

bool JsonTreeItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        JsonTreeItem *item = new JsonTreeItem(data, this);
        childItems.insert(position, item);
    }
    return true;
}

bool JsonTreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size()) {
        return false;
    }

    for (int row = 0; row < count; ++row) {
        delete childItems.takeAt(position);
    }

    return true;
}

bool JsonTreeItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size()) {
        return false;
    }

    itemData[column] = value;
    return true;
}
