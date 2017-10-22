#ifndef CQJsonTest_H
#define CQJsonTest_H

#include <QFrame>

class QVBoxLayout;
class CQJsonModel;
class CQJsonTable;
class CQJsonTree;

class CQJsonTest : public QFrame {
  Q_OBJECT

 public:
  CQJsonTest();

  const QString &match() const { return match_; }
  void setMatch(const QString &s) { match_ = s; }

  void load(const QString &filename, bool hier);

 private:
  QString      match_;
  QVBoxLayout *layout_ { nullptr };
  CQJsonModel *model_  { nullptr };
  CQJsonTable *table_  { nullptr };
  CQJsonTree  *tree_   { nullptr };
};

#endif
