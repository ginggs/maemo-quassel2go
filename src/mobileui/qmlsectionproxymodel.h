#ifndef QMLSECTIONPROXYMODEL_H
#define QMLSECTIONPROXYMODEL_H

#include <QAbstractProxyModel>

class QmlSectionProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit QmlSectionProxyModel(QObject *parent = 0);

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &proxyIndex, int role) const;
  //    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    virtual void setSourceModel ( QAbstractItemModel * sourceModel );

    void setSourceRootIndex(const QModelIndex &root);

//    void addMapping(int col, const QByteArray &role);
//    void removeMapping(int col);
//    void removeMapping(const QByteArray &role);
protected:
//    virtual void updateRoleMappings();
//    QModelIndex mapWithRole(const QModelIndex &idx, int role) const;
private:
  //  QMap<int, QByteArray> m_mappings;
    QModelIndex _root;

signals:

public slots:

};

#endif // QMLSECTIONPROXYMODEL_H