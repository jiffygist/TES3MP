#ifndef CSM_WOLRD_NESTEDCOLLECTION_H
#define CSM_WOLRD_NESTEDCOLLECTION_H

class QVariant;

namespace CSMWorld
{
    class NestableColumn;
    class NestedTableWrapperBase;

    class NestedCollection
    {

    public:

        NestedCollection();
        virtual ~NestedCollection();

        virtual void addNestedRow(int row, int col, int position) = 0;

        virtual void removeNestedRows(int row, int column, int subRow) = 0;

        virtual QVariant getNestedData(int row, int column, int subRow, int subColumn) const = 0;

        virtual void setNestedData(int row, int column, const QVariant& data, int subRow, int subColumn) = 0;

        virtual NestedTableWrapperBase* nestedTable(int row, int column) const = 0;

        virtual void setNestedTable(int row, int column, const NestedTableWrapperBase& nestedTable) = 0;

        virtual int getNestedRowsCount(int row, int column) const;

        virtual int getNestedColumnsCount(int row, int column) const;

        virtual NestableColumn *getNestableColumn(int column) = 0;
    };
}

#endif // CSM_WOLRD_NESTEDCOLLECTION_H
