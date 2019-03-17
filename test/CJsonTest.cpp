#include <CJson.h>

int
main(int argc, char **argv)
{
  CJson *json = new CJson;

  std::string filename;
  std::string match;

  bool typeFlag  = false;
  bool shortFlag = false;
  bool hierFlag  = false;
  bool nameFlag  = false;
  bool valueFlag = false;

  std::string hierName  = "children";
  std::string hierKey   = "name";
  std::string hierValue = "size";

  for (auto i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string arg(&argv[i][1]);

      if      (arg == "debug")
        json->setDebug(true);
      else if (arg == "quiet")
        json->setQuiet(true);
      else if (arg == "flat")
        json->setPrintFlat(true);
      else if (arg == "csv")
        json->setPrintCsv(true);
      else if (arg == "hier")
        hierFlag = true;
      else if (arg == "name")
        nameFlag = true;
      else if (arg == "value")
        valueFlag = true;
      else if (arg == "to_real")
        json->setStringToReal(true);
      else if (arg == "match")
        match = argv[++i];
      else if (arg == "type")
        typeFlag = true;
      else if (arg == "short")
        shortFlag = true;
      else if (arg == "hierName") {
        ++i;

        if (i < argc)
          hierName = argv[i];
      }
      else if (arg == "hierKey") {
        ++i;

        if (i < argc)
          hierKey = argv[i];
      }
      else if (arg == "hierValue") {
        ++i;

        if (i < argc)
          hierValue = argv[i];
      }
      else if (arg == "h" || arg == "help") {
        std::cerr << "CJsonTest [-debug] [-quiet] [-flat] [-csv] [-match <pattern>] "
                     "[-type] [-short] [-hier] [-name] [-value] "
                     "[-hierName <name>] [-hierKey <key>] [hierValue <value>] "
                     "<filename>\n";
        exit(0);
      }
      else
        std::cerr << "Unhandled option: " << arg << "\n";
    }
    else
      filename = argv[i];
  }

  if (filename == "")
    exit(1);

  CJson::ValueP value;

  if (! json->loadFile(filename.c_str(), value)) {
    std::cerr << "Parse failed\n";
    exit(1);
  }

  if (json->isDebug())
    std::cout << *value << "\n";

  if      (match != "") {
    CJson::Values values;

    if (! json->matchValues(value, match, values))
      exit(1);

    if (typeFlag) {
      for (const auto &v : values) {
        std::cout << v->hierTypeName() << "\n";
      }
    }
    else {
      if (json->isStringToReal()) {
        for (const auto &v : values) {
          v->printReal(std::cout);

          std::cout << "\n";
        }
      }
      else {
        for (const auto &v : values) {
          if      (typeFlag)
            std::cout << v->hierTypeName();
          else if (shortFlag)
            v->printShort(std::cout);
          else if (nameFlag)
            v->printName(std::cout);
          else if (valueFlag)
            v->printValue(std::cout);
          else
            v->print(std::cout);

          std::cout << "\n";
        }
      }
    }
  }
  else if (typeFlag) {
    std::cout << value->hierTypeName() << "\n";
  }
  else {
    typedef std::pair<std::string,std::string>   NameValue;
    typedef std::vector<NameValue>               NameValueArray;
    typedef std::map<std::string,NameValueArray> PackageNameValueArray;

    std::string           package;
    PackageNameValueArray packageNameValues;

    json->processNodes(value, [&package, &packageNameValues, &hierFlag, &hierName,
                               &hierKey, &hierValue]
     (const CJson::OptString & /*name*/, const CJson::ValueP v, int /*depth*/) {
      if (v->isObject()) {
        const CJson::Object *obj = v->cast<CJson::Object>();

        if (obj->hasName(hierName)) {
          CJson::ValueP nameValue;

          if (obj->getNamedValue(hierKey, nameValue)) {
            package = nameValue->cast<CJson::String>()->value();

            if (hierFlag) {
              CJson::Value *parent = nameValue->parent();

              if (parent && parent->isObject())
                parent = parent->parent();

              if (parent && parent->isArray())
                parent = parent->parent();

              while (parent && parent->isObject()) {
                const CJson::Object *pobj = parent->cast<CJson::Object>();

                CJson::ValueP keyValue;

                if (pobj->getNamedValue(hierKey, keyValue) && keyValue->isString()) {
                  const CJson::String *keyStr = keyValue->cast<CJson::String>();

                  package = keyStr->value() + "/" + package;
                }

                parent = parent->parent();

                if (parent && parent->isArray())
                  parent = parent->parent();
              }
            }
          }

          return true; // iterate children
        }

        //---

        CJson::ValueP keyValue, valueValue;

        if (! obj->getNamedValue(hierKey  , keyValue  ) ||
            ! obj->getNamedValue(hierValue, valueValue))
          return false;

        if (! keyValue->isString())
          return false;

        const CJson::String *keyStr = keyValue->cast<CJson::String>();

        if      (valueValue->isNumber()) {
          const CJson::Number *valueNum = valueValue->cast<CJson::Number>();

          NameValue nv(keyStr->value(), std::to_string(valueNum->value()));

          packageNameValues[package].push_back(nv);
        }
        else if (valueValue->isString()) {
          const CJson::String *valueStr = valueValue->cast<CJson::String>();

          NameValue nv(keyStr->value(), valueStr->value());

          packageNameValues[package].push_back(nv);
        }
        else
          return false;

        return false; // stop iteration
      }

      return true;
    });

    if (hierFlag) {
      for (const auto &pnv : packageNameValues) {
        for (const auto &nv : pnv.second) {
           std::cout << pnv.first << "/" << nv.first << "\t" << nv.second << "\n";
        }
      }
    }
    else {
      for (const auto &pnv : packageNameValues) {
        for (const auto &nv : pnv.second) {
           std::cout << nv.first << "\t" << nv.second << "\t" << pnv.first << "\n";
        }
      }
    }
  }

  delete json;

  exit(0);
}
