#ifndef CQJsonTree_H
#define CQJsonTree_H

#include <CQTreeView.h>

class CQJsonModel;

class CQJsonTree : public CQTreeView {
  Q_OBJECT

 public:
  CQJsonTree(QWidget *parent=nullptr);

  void setJsonModel(CQJsonModel *model);

 private:
  CQJsonModel* model_ { nullptr };
};

#endif
