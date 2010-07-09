import Qt 4.7

ListView {
  id: flickable
  anchors.fill: parent

  model: myModel
  delegate: Text { text: plaintext; wrapMode: Text.Wrap; width: parent.width }
  contentY:  10000

  Rectangle {
    id: scrollbar
    anchors.right: flickable.right
    y: flickable.visibleArea.yPosition * flickable.height
           width: 10
           height: flickable.visibleArea.heightRatio * flickable.height
           color: "black"
  }
}
