/***************************************************************************
 *   Copyright (C) 2005-2010 by the Quassel Project                        *
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

#ifndef COLUMNHANDLEITEM_H_
#define COLUMNHANDLEITEM_H_

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPropertyAnimation>

#include "chatscene.h"

class ColumnHandleItem : public QGraphicsObject {
  Q_OBJECT
  Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

  Q_PROPERTY(qreal columnPos READ x WRITE setXPos NOTIFY positionChanged)

public:
  ColumnHandleItem(qreal width, QGraphicsItem *parent = 0);
  virtual inline int type() const { return ChatScene::ColumnHandleType; }

  inline qreal width() const { return _width; }
  inline QRectF boundingRect() const { return _boundingRect; }
  inline qreal sceneLeft() const { return _sceneLeft; }
  inline qreal sceneRight() const { return _sceneRight; }

  inline qreal opacity() const { return _opacity; }

  void setXPos(qreal xpos);
  void setXLimits(qreal min, qreal max);

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

public slots:
  void sceneRectChanged(const QRectF &);
  void setOpacity(qreal opacity);

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
  void positionChanged(qreal x);

private:
  qreal _width;
  qreal _sceneLeft, _sceneRight;
  QRectF _boundingRect;
  bool _moving;
  qreal _offset;
  qreal _minXPos, _maxXPos;
  qreal _opacity;
  QPropertyAnimation *_animation;
};

#endif
