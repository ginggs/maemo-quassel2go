#include <qdeclarative.h>
#include <QAbstractItemModel>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QListWidget>
#include <QStandardItem>
#include <QStandardItemModel>

#include "qmlchatview.h"

#include "topicmodel.h"
#include "qmlcontextobject.h"
//#include "chatview.h"
//#include "bufferwidget.h"
//#include "qmlchatview.h"
#include "qmlscrollmodel.h"
#include "qmlinputwidget.h"
#include "qmlquasselplugin.h"
#include "qmlthemeimageprovider.h"
#include "iconloader.h"

#include <QDebug>

class MyStandardItemModel : public QStandardItemModel
{
public:
  MyStandardItemModel(QObject *parent) : QStandardItemModel(parent)
  {
  }

  void setRoleNames(const QHash<int, QByteArray> &roleNames) { QStandardItemModel::setRoleNames(roleNames); };
};

QmlQuasselPlugin::QmlQuasselPlugin(QObject *parent)
  : QDeclarativeExtensionPlugin(parent)
{
  qDebug() << "QmlQuasselPlugin:: create";
}

QmlQuasselPlugin::~QmlQuasselPlugin()
{
}

void QmlQuasselPlugin::initializeEngine(QDeclarativeEngine * engine, const char * uri)
{
  qDebug() << "QmlQuasselPlugin::initializeEngine" << uri;
  Q_ASSERT(uri == QLatin1String("org.quassel"));

  // initializations...
  //  BufferWidget *_bufferWidget = new BufferWidget(0);
  //  _bufferWidget->setModel(Client::bufferModel());
  //  _bufferWidget->setSelectionModel(Client::bufferModel()->standardSelectionModel());

  // fake buffer widget for debugging
  QListWidget *w = new QListWidget(0);
  w->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  for(int i = 0; i < 50; ++i)
    w->addItem(new QListWidgetItem(QString("Test %1").arg(i), w));
  //_scrollModel->setScrollArea(w);

  // fake buffer list model
  MyStandardItemModel *bufferModel = new MyStandardItemModel(this);
  for(int i = 0; i < 3; ++i) {
    QStandardItem *category = new QStandardItem();
    category->setData(QString("Network %1").arg(i), Qt::DisplayRole);
    for(int j = 0; j < 8; ++j) {
      QStandardItem *buf = new QStandardItem();
      buf->setData(QString("Buffer %1").arg(j), Qt::DisplayRole);
      buf->setData("irc-channel-active", NetworkModel::DecorationIconNameRole);
      category->appendRow(buf);
    }
    bufferModel->appendRow(category);
  }
  QHash<int, QByteArray> roles = bufferModel->roleNames();
  // roles[Qt::DisplayRole] = "display"
  roles[NetworkModel::BufferTypeRole] = "bufferType";
  roles[NetworkModel::ItemActiveRole] = "itemActive";
  roles[NetworkModel::BufferActivityRole] = "bufferActivity";
  roles[NetworkModel::BufferIdRole] = "bufferId";
  roles[NetworkModel::NetworkIdRole] = "networkId";
  roles[NetworkModel::BufferInfoRole] = "bufferInfo";
  roles[NetworkModel::ItemTypeRole] = "itemType";
  roles[NetworkModel::UserAwayRole] = "userAway";
  roles[NetworkModel::IrcUserRole] = "ircUser";
  roles[NetworkModel::IrcChannelRole] = "ircChannel";
  roles[NetworkModel::BufferFirstUnreadMsgIdRole] = "bufferFirstUnreadMsg";
  roles[NetworkModel::MarkerLineMsgIdRole] = "markerLineMsgId";
  roles[NetworkModel::DecorationIconNameRole] = "decorationIconName";
  bufferModel->setRoleNames(roles);

  QmlChatView::setBufferWidget(w /*_bufferWidget*/);
  QmlInputWidget::setEmbeddedWidget(0 /*_inputWidget*/);

  qDebug() << "current icon theme" << IconLoader::global()->theme();
  QmlThemeImageProvider *imgProvider = new QmlThemeImageProvider(engine);
  engine->addImageProvider(QLatin1String("quassel"), imgProvider);

  qDebug() << "imgProvider test..." << (imgProvider->requestPixmap("irc-channel-active", 0, QSize(16,16)).size());

  QmlContextObject *_qmlContextObject = new QmlContextObject(this);

  _qmlContextObject->setAllBuffersModel(bufferModel);
  _qmlContextObject->setCurrentBufferIndex(0);
  qDebug() << "_qmlContextObject allBufMdl" << _qmlContextObject->allBuffersModel() << " idx:" << _qmlContextObject->currentBufferIndex();
  engine->rootContext()->setContextObject(_qmlContextObject);
  engine->rootContext()->setContextProperty("ctxt", _qmlContextObject);
  engine->rootContext()->setContextProperty("topicModel", new TopicModel(this) /*_topicModel*/);
  engine->rootContext()->setContextProperty("test", "abc");
}

void QmlQuasselPlugin::registerTypes(const char *uri)
{
  qDebug() << "QmlQuasselPlugin::registerTypes" << uri;

  Q_ASSERT(uri == QLatin1String("org.quassel"));

  //qmlRegisterType<ChatView>();
  //qmlRegisterType<BufferWidget>();
  qmlRegisterType<TopicModel>();
  qmlRegisterType<QAbstractItemModel>();
  qmlRegisterType<QmlScrollModel>();
  qmlRegisterType<QmlChatView>(uri, 0, 1, "QuasselChatView");
  qmlRegisterType<QmlInputWidget>(uri, 0, 1, "QuasselInputWidget");
}

Q_EXPORT_PLUGIN2(qmlquasselplugin, QmlQuasselPlugin);
