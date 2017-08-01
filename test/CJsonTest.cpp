#include <CJson.h>

int
main(int argc, char **argv)
{
  CJson json;

  std::string filename;
  std::string match;
  bool        typeFlag = false;
  bool        shortFlag = false;

  for (auto i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string arg(&argv[i][1]);

      if      (arg == "debug")
        json.setDebug(true);
      else if (arg == "quiet")
        json.setQuiet(true);
      else if (arg == "flat")
        json.setPrintFlat(true);
      else if (arg == "to_real")
        json.setStringToReal(true);
      else if (arg == "match")
        match = argv[++i];
      else if (arg == "type")
        typeFlag = true;
      else if (arg == "short")
        shortFlag = true;
      else if (arg == "h" || arg == "help") {
        std::cerr << "CJsonTest [-debug] [-quiet] [-flat] [-match <pattern>] <filename>" <<
                     std::endl;
        exit(0);
      }
      else
        std::cerr << "Unhandled option: " << arg << std::endl;
    }
    else
      filename = argv[i];
  }

  if (filename == "")
    exit(1);

  CJson::Value *value;

  if (! json.loadFile(filename.c_str(), value)) {
    std::cerr << "Parse failed" << std::endl;
    exit(1);
  }

  if (json.isDebug())
    std::cout << *value << std::endl;

  if      (match != "") {
    CJson::Array::Values values;

    if (! json.matchValues(value, match, values))
      exit(1);

    if (typeFlag) {
      for (const auto &v : values) {
        std::cout << v->hierTypeName() << std::endl;
      }
    }
    else {
      if (json.isStringToReal()) {
        for (const auto &v : values) {
          v->printReal(std::cout);

          std::cout << std::endl;
        }
      }
      else {
        for (const auto &v : values) {
          if      (typeFlag)
            std::cout << v->hierTypeName();
          else if (shortFlag)
            v->printShort(std::cout);
          else
            v->print(std::cout);

          std::cout << std::endl;
        }
      }
    }
  }
  else if (typeFlag) {
    std::cout << value->hierTypeName() << std::endl;
  }
  else {
    typedef std::pair<std::string,double>        NameValue;
    typedef std::vector<NameValue>               NameValueArray;
    typedef std::map<std::string,NameValueArray> PackageNameValueArray;

    std::string           package;
    PackageNameValueArray packageNameValues;

    json.processNodes(value, [&package, &packageNameValues]
     (const COptString & /*name*/, const CJson::Value *v, int /*depth*/) {
      if (v->isObject()) {
        const CJson::Object *obj = v->cast<CJson::Object>();

        if (obj->hasName("children")) {
          CJson::Value *nameValue;

          if (obj->getNamedValue("name", nameValue))
            package = nameValue->cast<CJson::String>()->value();

          return true;
        }

        //---

        CJson::Value *nameValue, *sizeValue;

        if (! obj->getNamedValue("name", nameValue) || ! obj->getNamedValue("size", sizeValue))
          return false;

        if (! nameValue->isString() || ! sizeValue->isNumber())
          return false;

        const CJson::String *nameStr = nameValue->cast<CJson::String>();
        const CJson::Number *sizeNum = sizeValue->cast<CJson::Number>();

        NameValue nv(nameStr->value(), sizeNum->value());

        packageNameValues[package].push_back(nv);

        return false;
      }

      return true;
    });

    for (const auto &pnv : packageNameValues) {
      for (const auto &nv : pnv.second) {
         std::cout << nv.first << "\t" << nv.second << "\t" << pnv.first << std::endl;
      }
    }
  }

  exit(0);
}
