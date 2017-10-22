#include <CQJsonTest.h>
#include <CQJsonTable.h>
#include <CQJsonTree.h>
#include <CQJsonModel.h>

#include <QApplication>
#include <QToolButton>
#include <QVBoxLayout>
#include <iostream>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  std::vector<QString> filenames;

  QString match;
  bool    hier = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string arg = &argv[i][1];

      if      (arg == "match") {
        ++i;

        if (i < argc)
          match = argv[i];
      }
      else if (arg == "hier") {
        hier = true;
      }
      else {
        std::cerr << "Invalid option '" << arg << "'" << std::endl;
      }
    }
    else {
      filenames.push_back(argv[i]);
    }
  }

  if (filenames.empty())
    exit(1);

  //---

  CQJsonTest test;

  if (match != "")
    test.setMatch(match);

  test.load(filenames[0], hier);

  test.show();

  app.exec();

  return 0;
}

//-----

CQJsonTest::
CQJsonTest()
{
  layout_ = new QVBoxLayout(this);
}

void
CQJsonTest::
load(const QString &filename, bool hier)
{
  model_ = new CQJsonModel;

  model_->load(filename);

  if (match_ != "")
    model_->applyMatch(match());

  if (model_->isHierarchical() || hier) {
    tree_ = new CQJsonTree;

    layout_->addWidget(tree_);

    tree_->setModel(model_);
  }
  else {
    table_ = new CQJsonTable;

    layout_->addWidget(table_);

    table_->setModel(model_);
  }
}
