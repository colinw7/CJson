#include <CQJsonTree.h>
#include <CQJsonModel.h>
#include <QHeaderView>

CQJsonTree::
CQJsonTree(QWidget *parent) :
 CQTreeView(parent)
{
  setObjectName("table");

  header()->setStretchLastSection(true);
}

void
CQJsonTree::
setJsonModel(CQJsonModel *model)
{
  model->setFlat(false);

  QTreeView::setModel(model);
}
