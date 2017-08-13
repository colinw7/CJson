#ifndef CQJsonTree_H
#define CQJsonTree_H

#include <QTreeView>

class CQJsonModel;
class CQHeaderView;

class CQJsonTree : public QTreeView {
  Q_OBJECT

 public:
  CQJsonTree(QWidget *parent=nullptr);

  void setModel(CQJsonModel *model);

 private:
  CQJsonModel*  model_  { nullptr };
  CQHeaderView* header_ { nullptr };
};

#endif
