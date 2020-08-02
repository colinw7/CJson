#include <CQJsonTree.h>
#include <CQJsonModel.h>

CQJsonTree::
CQJsonTree(QWidget *parent) :
 CQTreeView(parent)
{
  setObjectName("table");
}

void
CQJsonTree::
setJsonModel(CQJsonModel *model)
{
  model->setFlat(false);

  QTreeView::setModel(model);
}
