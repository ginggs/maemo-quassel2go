/***************************************************************************
 *   Copyright (C) 2005-09 by the Quassel Project                          *
 *   devel@quassel-irc.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3.                                           *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef MAINWIN_H_
#define MAINWIN_H_

#ifdef HAVE_KDE
#  include <KMainWindow>
#else
#  include <QMainWindow>
#endif

#include "qtui.h"
#include "titlesetter.h"
#include "uisettings.h"

class QmlContextObject;

class ActionCollection;
class BufferHotListFilter;
class MobileBufferView;
class BufferViewConfig;
class ClientBufferViewConfig;
class CoreAccount;
class CoreConnectionStatusWidget;
class BufferWidget;
class InputWidget;
class MsgProcessorStatusWidget;
class NickListWidget;
class SystemTray;
class ChatMonitorView;
class TopicWidget;
class TopicModel;

class QMenu;
class QLabel;
class QToolBar;

class KHelpMenu;

//!\brief The main window of Quassel's QtUi.
class MainWin
#ifdef HAVE_KDE
: public KMainWindow {
#else
: public QMainWindow {
#endif
  Q_OBJECT

  public:
    MainWin(QWidget *parent = 0);
    virtual ~MainWin();

    void init();

    void addBufferView(ClientBufferViewConfig *config);
    MobileBufferView *allBuffersView() const;
    MobileBufferView *activeBufferView() const;

    inline BufferWidget *bufferWidget() const { return _bufferWidget; }
    inline SystemTray *systemTray() const { return _systemTray; }

    bool event(QEvent *event);

    static void flagRemoteCoreOnly(QObject *object) { object->setProperty("REMOTE_CORE_ONLY", true); }
    static bool isRemoteCoreOnly(QObject *object) { return object->property("REMOTE_CORE_ONLY").toBool(); }

    void saveStateToSettings(UiSettings &);
    void restoreStateFromSettings(UiSettings &);

  public slots:
    void showStatusBarMessage(const QString &message);
    void nextBufferView();     //!< Activate the next bufferview
    void previousBufferView(); //!< Activate the previous bufferview
    void nextBuffer();
    void previousBuffer();

    //! Quit application
    void quit();

  protected:
    void closeEvent(QCloseEvent *event);
    void moveEvent(QMoveEvent *event);
    void resizeEvent(QResizeEvent *event);

  protected slots:
    void connectedToCore();
    void setConnectedState();
    void disconnectedFromCore();
    void setDisconnectedState();

  private slots:
    void addBufferView(int bufferViewConfigId);
    void awayLogDestroyed();
    void removeBufferView(int bufferViewConfigId);
    void currentBufferChanged(BufferId);
    void messagesInserted(const QModelIndex &parent, int start, int end);
    void showAboutDlg();
    void showChannelList(NetworkId netId = NetworkId());
    void showCoreConnectionDlg();
    void showCoreConfigWizard(const QVariantList &);
    void showCoreInfoDlg();
    void showAwayLog();
    void showSettingsDlg();
    void showNotificationsDlg();
    void showIgnoreList(QString newRule = QString());
    void showShortcutsDlg();
    void toggleFullscreen();
    void setFullscreen(bool fullScreen);

    void handleCoreConnectionError(const QString &errorMsg);
    void userAuthenticationRequired(CoreAccount *, bool *valid, const QString &errorMessage);
    void handleNoSslInClient(bool *accepted);
    void handleNoSslInCore(bool *accepted);
#ifdef HAVE_SSL
    void handleSslErrors(const QSslSocket *socket, bool *accepted, bool *permanently);
#endif

    void on_actionConfigureNetworks_triggered();
    void on_actionConfigureViews_triggered();
    void on_jumpHotBuffer_triggered();
    void on_actionDebugNetworkModel_triggered();
    void on_actionDebugBufferViewOverlay_triggered();
    void on_actionDebugMessageModel_triggered();
    void on_actionDebugHotList_triggered();
    void on_actionDebugLog_triggered();

    void bindJumpKey();
    void onJumpKey();

    void clientNetworkCreated(NetworkId);
    void clientNetworkRemoved(NetworkId);
    void clientNetworkUpdated();
    void connectOrDisconnectFromNet();

    void saveStatusBarStatus(bool enabled);

    void loadLayout();
    void saveLayout();

    void bufferViewToggled(bool enabled);
    void changeActiveBufferView(bool backwards);
    void changeActiveBufferView(int bufferViewId);

  signals:
    void connectToCore(const QVariantMap &connInfo);
    void disconnectFromCore();

  private:
#ifdef HAVE_KDE
    KHelpMenu *_kHelpMenu;
#endif

    MsgProcessorStatusWidget *_msgProcessorStatusWidget;
    CoreConnectionStatusWidget *_coreConnectionStatusWidget;
    SystemTray *_systemTray;

    TitleSetter _titleSetter;

    void setupActions();
    void setupBufferWidget();
    void setupMenus();
    void setupNickWidget();
    void setupChatMonitor();
    void setupInputWidget();
    void setupTopicWidget();
    void setupViewMenuTail();
    void setupStatusBar();
    void setupSystray();
    void setupTitleSetter();
    void setupHotList();
    void setupLock();

    void updateIcon();
    void enableMenus();

    class QDeclarativeView *_declarativeView;
    QmlContextObject *_qmlContextObject;

    QList<MobileBufferView *> _bufferViews;
    BufferWidget *_bufferWidget;
    NickListWidget *_nickListWidget;
    InputWidget *_inputWidget;
    ChatMonitorView *_chatMonitorView;
    TopicModel *_topicModel;

    QToolBar *_mainToolBar, *_chatViewToolBar, *_nickToolBar;

    QWidget *_awayLog;

    bool _layoutLoaded;

    QSize _normalSize; //!< Size of the non-maximized window
    QPoint _normalPos; //!< Position of the non-maximized window

    BufferHotListFilter *_bufferHotList;
    QHash<int, BufferId> _jumpKeyMap;
    int _activeBufferViewIndex;

    friend class QtUi;
};

#endif
