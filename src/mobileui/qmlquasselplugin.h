#ifndef QMLQUASSELPLUGIN_H
#define QMLQUASSELPLUGIN_H

#include <QDeclarativeExtensionPlugin>
#include <QObject>

class QmlQuasselPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
  QmlQuasselPlugin(QObject *parent = 0);
  virtual ~QmlQuasselPlugin();

  void	initializeEngine ( QDeclarativeEngine * engine, const char * uri );
  void registerTypes(const char *uri);

signals:

public slots:

protected slots:
  void timeout();

private:
  class QmlContextObject *_qmlContextObject;
  class MyStandardItemModel *_bufferModel;

};

#endif // QMLQUASSELPLUGIN_H
