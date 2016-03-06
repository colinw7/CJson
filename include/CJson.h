#ifndef CJSON_H
#define CJSON_H

#include <cstdio>
#include <cassert>
#include <iostream>
#include <vector>
#include <map>

#include <COptVal.h>

namespace CJson {
  enum ValueType {
    VALUE_STRING,
    VALUE_NUMBER,
    VALUE_TRUE,
    VALUE_FALSE,
    VALUE_NULL,
    VALUE_OBJECT,
    VALUE_ARRAY,
  };

  //---

  // Json Value base class
  class Value {
   public:
    Value(ValueType type) : type_(type) { }

    virtual ~Value() { }

    ValueType type() const { return type_; }

    bool isString() const { return type_ == VALUE_STRING; }
    bool isNumber() const { return type_ == VALUE_NUMBER; }
    bool isTrue  () const { return type_ == VALUE_TRUE  ; }
    bool isFalse () const { return type_ == VALUE_FALSE ; }
    bool isNull  () const { return type_ == VALUE_NULL  ; }
    bool isObject() const { return type_ == VALUE_OBJECT; }
    bool isArray () const { return type_ == VALUE_ARRAY ; }

    bool isComposite() const { return isObject() || isArray(); }

    template<typename T>
    T *cast() {
      T *t = dynamic_cast<T *>(this);
      assert(t);

      return t;
    }

    template<typename T>
    const T *cast() const {
      const T *t = dynamic_cast<const T *>(this);
      assert(t);

      return t;
    }

    virtual const char *typeName() const { return "value"; }

    virtual void print(std::ostream &os=std::cout) const = 0;

    virtual void printReal(std::ostream &os=std::cout) const { print(os); }

    virtual void printShort(std::ostream &os=std::cout) const { print(os); }

    friend std::ostream &operator<<(std::ostream &os, const Value &v) {
      v.print(os);

      return os;
    }

   protected:
    ValueType type_;
  };

  //---

  // Json String
  class String : public Value {
   public:
    String(const std::string &str) :
     Value(VALUE_STRING), str_(str) {
    }

    const std::string &value() const { return str_; }

    bool toReal(double &r) const;

    const char *typeName() const override { return "string"; }

    void printReal(std::ostream &os=std::cout) const override;

    void printShort(std::ostream &os=std::cout) const override;

    void print(std::ostream &os=std::cout) const override;

  private:
    std::string str_;
  };

  //---

  // Json Number
  class Number : public Value {
   public:
    Number(double value) :
     Value(VALUE_NUMBER), value_(value) {
    }

    double value() const { return value_; }

    const char *typeName() const override { return "number"; }

    void print(std::ostream &os=std::cout) const override;

  private:
    double value_;
  };

  //---

  // Json True
  class True : public Value {
   public:
    True() :
     Value(VALUE_TRUE) {
    }

    bool value() const { return true; }

    const char *typeName() const override { return "true"; }

    void print(std::ostream &os=std::cout) const override;
  };

  //---

  // Json False
  class False : public Value {
   public:
    False() :
     Value(VALUE_FALSE) {
    }

    bool value() const { return false; }

    const char *typeName() const override { return "false"; }

    void print(std::ostream &os=std::cout) const override;
  };

  //---

  // Json Null
  class Null : public Value {
   public:
    Null() :
     Value(VALUE_NULL) {
    }

    void *value() const { return nullptr; }

    const char *typeName() const override { return "null"; }

    void print(std::ostream &os=std::cout) const override;
  };

  //---

  template<typename T>
  struct TypeMap {
    typedef CJson::Null Type;
  };

  template<>
  struct TypeMap<std::string> {
    typedef CJson::String Type;
  };

  template<>
  struct TypeMap<double> {
    typedef CJson::Number Type;
  };

  //---

  // Json Object (name/value map)
  class Object : public Value {
   public:
    typedef std::map<std::string,Value *>  NameValueMap;
    typedef std::pair<std::string,Value *> NameValue;
    typedef std::vector<NameValue>         NameValueArray;

   public:
    Object() :
     Value(VALUE_OBJECT) {
    }

   ~Object() {
      for (auto &nv : nameValueArray_)
        delete nv.second;
    }

    const NameValueMap &nameValueMap() const { return nameValueMap_; }

    const NameValueArray &nameValueArray() const { return nameValueArray_; }

    bool hasName(const std::string &name) const {
      NameValueMap::const_iterator p = nameValueMap_.find(name);

      return (p != nameValueMap_.end());
    }

    void setNamedValue(const std::string &name, Value *value) {
      NameValueMap::iterator p = nameValueMap_.find(name);

      nameValueMap_[name] = value;

      nameValueArray_.push_back(NameValue(name, value));
    }

    void getNames(std::vector<std::string> &names) {
      for (const auto &nv : nameValueArray_)
        names.push_back(nv.first);
    }

    void getValues(std::vector<Value *> &names) {
      for (const auto &nv : nameValueArray_)
        names.push_back(nv.second);
    }

    bool getNamedValue(const std::string &name, Value *&value) const {
      NameValueMap::const_iterator p = nameValueMap_.find(name);

      if (p == nameValueMap_.end())
        return false;

      value = (*p).second;

      return true;
    }

    template<typename T>
    bool getNamedValueT(const std::string &name, T *&t) const {
      Value *value;

      if (! getNamedValue(name, value))
        return false;

      t = dynamic_cast<T *>(value);

      if (! t)
        return false;

      return true;
    }

    const char *typeName() const override { return "object"; }

    void printReal(std::ostream &os=std::cout) const override;

    void print(std::ostream &os=std::cout) const override;

   private:
    NameValueMap   nameValueMap_;
    NameValueArray nameValueArray_;
  };

  //---

  // Json Array
  class Array : public Value {
   public:
    typedef std::vector<Value *> Values;

   public:
    Array() :
     Value(VALUE_ARRAY) {
    }

   ~Array() {
      for (auto &v : values_)
        delete v;
    }

    const Values &values() const { return values_; }

    void addValue(Value *value) {
      values_.push_back(value);
    }

    uint size() const { return values_.size(); }

    Value *at(uint i) const { return values_[i]; }

    template<typename T>
    T *atT(uint i) const {
      T *t = dynamic_cast<T *>(values_[i]);

      return t;
    }

    const char *typeName() const override { return "array"; }

    void printReal(std::ostream &os=std::cout) const override;

    void print(std::ostream &os=std::cout) const override;

   private:
    Values values_;
  };

  //---

  void setDebug(bool b);
  bool isDebug();

  //---

  void setQuiet(bool b);
  bool isQuiet();

  //---

  void setPrintFlat(bool b);
  bool isPrintFlat();

  //---

  void setStringToReal(bool b);
  bool isStringToReal();

  //---

  void skipSpace(const std::string &str, int &i);

  double stod(const std::string &str, bool &ok);
  long   stol(const std::string &str, bool &ok);

  // read string at file pos
  bool readString(const std::string &str, int &i, std::string &str1);

  // read number at file pos
  bool readNumber(const std::string &str, int &i, std::string &str1);

  // read object at file pos
  bool readObject(const std::string &str, int &i, Object *&obj);

  // read array at file pos
  bool readArray(const std::string &str, int &i, Array *&array);

  // read value at file pos
  bool readValue(const std::string &str, int &i, Value *&value);

  bool readLine(FILE *fp, std::string &line);

  // load file and return root value
  bool loadFile(const std::string &filename, Value *&value);

  // load file and return typed root value
  template<typename T>
  bool loadFileT(const std::string &filename, T *&value) {
    Value *value1;

    if (! loadFile(filename.c_str(), value1))
      return false;

    value = dynamic_cast<T *>(value1);

    if (! value)
      return false;

    return true;
  }

  template<typename T, typename FUNC>
  bool processValues(const Object *obj, const std::string &name, const FUNC &f) {
    auto p = name.find('/');

    if (p != std::string::npos) {
      std::string lhs = name.substr(0, p);
      std::string rhs = name.substr(p + 1);

      Array *nodes;

      if (! obj->getNamedValueT<Array>(lhs, nodes))
        return false;

      for (uint i = 0; i < nodes->size(); ++i) {
        Object *node = nodes->atT<Object>(i);
        if (! node) continue;

        if (! processValues<T,FUNC>(node, rhs, f))
          continue;
      }
    }
    else {
      typedef typename TypeMap<T>::Type Type;

      Type *v;

      if (! obj->getNamedValueT<Type>(name, v))
        return false;

      f(v->value());
    }

    return true;
  }

  template<typename T>
  bool getValues(const Object *obj, const std::string &name, std::vector<T> &values) {
    auto f = [&](const T &value) { values.push_back(value); };

    return processValues<T,decltype(f)>(obj, name, f);
  }

  template<typename FUNC>
  void processNodes(const Value *value, const FUNC &f) {
    return processNameNodes(COptString(), value, 0, f);
  }

  template<typename FUNC>
  void processNameNodes(const COptString &name, const Value *value, int depth, const FUNC &f) {
    if (! f(name, value, depth))
      return;

    switch (value->type()) {
      case VALUE_OBJECT: {
        auto obj = value->cast<Object>();

        for (const auto &nv : obj->nameValueArray())
          processNameNodes(COptString(nv.first), nv.second, depth + 1, f);

        break;
      }
      case VALUE_ARRAY : {
        auto array = value->cast<Array>();

        for (const auto &v : array->values())
          processNameNodes(COptString(), v, depth + 1, f);

        break;
      }
      default:
        break;
    }
  }

  //------

  /* match values:
   *  fields are separated by slash '/'
   *  values can be grouped using braces {<match>,<match>,...}
   *  arrays are added using square brackets with optional index range [<start>:<end>]
   *  list of object keys can be returned using ?
   *  array index can be added using #
   *
   *  e.g. "head/[1:3]/{name1,name2}/?
   */
  bool matchValues(Value *value, const std::string &match, Array::Values &values);

  bool matchValues(Value *value, int i, const std::string &match, Array::Values &values);

  bool matchObject(Value *value, const std::string &match, Value* &value1);

  bool matchArray(Value *value, const std::string &lhs, const std::string &rhs,
                  Array::Values &values);
  bool matchList(Value *value, int ind, const std::string &lhs, const std::string &rhs,
                  Array::Values &values);

  bool matchHier(Value *value, int ind, const std::string &lhs, const std::string &rhs,
                 Array::Values &values);
  bool matchHier1(Value *value, int ind, const std::string &lhs, const std::string &rhs,
                  const std::vector<std::string> &keys, Array::Values &ivalues,
                  Array::Values &values);

  String *hierValuesToKey(const Array::Values &values, const Array::Values &kvalues);

  //------

  String* createString(const std::string &str);
  Number* createNumber(double r);
  True*   createTrue();
  False*  createFalse();
  Null*   createNull();
  Object* createObject();
  Array*  createArray();
}

#endif
