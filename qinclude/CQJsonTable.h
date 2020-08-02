#ifndef CQJsonTable_H
#define CQJsonTable_H

#include <QTableView>

class CQJsonModel;
class CQHeaderView;

class CQJsonTable : public QTableView {
  Q_OBJECT

 public:
  CQJsonTable(QWidget *parent=nullptr);

  void setModel(QAbstractItemModel *model) override;

 private:
  CQJsonModel*  model_  { nullptr };
  CQHeaderView* header_ { nullptr };
};

#endif
