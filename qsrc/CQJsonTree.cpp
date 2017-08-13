#include <CQJsonTree.h>
#include <CQJsonModel.h>
#include <CQHeaderView.h>

CQJsonTree::
CQJsonTree(QWidget *parent) :
 QTreeView(parent)
{
  setObjectName("table");

  header_ = new CQHeaderView(this);

  setHeader(header_);
}

void
CQJsonTree::
setModel(CQJsonModel *model)
{
  QTreeView::setModel(model);
}
