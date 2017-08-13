#include <CQJsonTable.h>
#include <CQJsonModel.h>
#include <CQHeaderView.h>

CQJsonTable::
CQJsonTable(QWidget *parent) :
 QTableView(parent)
{
  setObjectName("table");

  header_ = new CQHeaderView(this);

  setHorizontalHeader(header_);
}

void
CQJsonTable::
setModel(CQJsonModel *model)
{
  QTableView::setModel(model);
}
