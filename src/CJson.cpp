#include <CJson.h>

namespace CJson {

static bool debug        = false;
static bool quiet        = false;
static bool printFlat    = false;
static bool stringToReal = false;

void setDebug(bool b) { debug = b; }
bool isDebug() { return debug; }

void setQuiet(bool b) { quiet = b; }
bool isQuiet() { return quiet; }

void setPrintFlat(bool b) { printFlat = b; }
bool isPrintFlat() { return printFlat; }

void setStringToReal(bool b) { stringToReal = b; }
bool isStringToReal() { return stringToReal; }

}

//------

void
CJson::
skipSpace(const std::string &str, int &i)
{
  int len = str.size();

  while (i < len && isspace(str[i]))
    ++i;
}

double
CJson::
stod(const std::string &str, bool &ok)
{
  char *p;

  double r = strtod(str.c_str(), &p);

  while (*p && isspace(*p))
    ++p;

  ok = (*p == '\0');

  return r;
}

long
CJson::
stol(const std::string &str, bool &ok)
{
  char *p;

  int i = strtol(str.c_str(), &p, 10);

  while (*p && isspace(*p))
    ++p;

  ok = (*p == '\0');

  return i;
}

// read string at file pos
bool
CJson::
readString(const std::string &str, int &i, std::string &str1)
{
  int len = str.size();

  if (i >= len || str[i] != '\"') return false;

  ++i;

  while (i < len) {
    if      (str[i] == '\\' && i < len - 1) {
      ++i;

      char c = str[i++];

      switch (c) {
        default : str1 += c; break;
        case 'b': str1 += '\b'; break;
        case 'f': str1 += '\f'; break;
        case 'n': str1 += '\n'; break;
        case 'r': str1 += '\r'; break;
        case 'y': str1 += '\t'; break;
        case 'u': {
          // TODO - 4 hexadecimal digits
          break;
        }
      }
    }
    else if (str[i] == '\"')
      break;
    else
      str1 += str[i++];
  }

  if (i >= len || str[i] != '\"')
    return false;

  ++i;

  return true;
}

// read numner at file pos
bool
CJson::
readNumber(const std::string &str, int &i, std::string &str1)
{
  int len = str.size();

  if (i >= len) return false;

  if (str[i] == '-')
    str1 += str[i++];

  if      (i < len && str[i] == '0')
    str1 += str[i++];
  else if (i < len && isdigit(str[i])) {
    while (i < len && isdigit(str[i]))
      str1 += str[i++];
  }
  else
    return false;

  if (i < len && str[i] == '.') {
    str1 += str[i++];

    while (i < len && isdigit(str[i]))
      str1 += str[i++];
  }

  if (i < len && (str[i] == 'e' || str[i] == 'E')) {
    str1 += str[i++];

    if (i < len && (str[i] == '+' || str[i] == '-'))
      str1 += str[i++];

    while (i < len && isdigit(str[i]))
      str1 += str[i++];
  }

  return true;
}

// read object at file pos
bool
CJson::
readObject(const std::string &str, int &i, Object *&obj)
{
  int len = str.size();

  if (i >= len || str[i] != '{')
    return false;

  obj = new Object;

  ++i;

  while (i < len) {
    skipSpace(str, i);

    std::string name;

    if (! readString(str, i, name)) {
      delete obj;
      return false;
    }

    skipSpace(str, i);

    if (i >= len || str[i] != ':') {
      delete obj;
      return false;
    }

    ++i;

    skipSpace(str, i);

    Value *value;

    if (! readValue(str, i, value)) {
      delete obj;
      return false;
    }

    skipSpace(str, i);

    obj->setNamedValue(name, value);

    if (i >= len || str[i] != ',')
      break;

    ++i;
  }

  if (i >= len || str[i] != '}') {
    delete obj;
    return false;
  }

  ++i;

  return true;
}

// read array at file pos
bool
CJson::
readArray(const std::string &str, int &i, Array *&array)
{
  int len = str.size();

  if (i >= len || str[i] != '[')
    return false;

  array = new Array;

  ++i;

  while (i < len) {
    skipSpace(str, i);

    if (str[i] == ']')
      break;

    Value *value;

    if (! readValue(str, i, value)) {
      delete array;
      return false;
    }

    array->addValue(value);

    skipSpace(str, i);

    if (i >= len || str[i] != ',')
      break;

    ++i;
  }

  if (i >= len || str[i] != ']') {
    delete array;
    return false;
  }

  ++i;

  return true;
}

// read value at file pos
bool
CJson::
readValue(const std::string &str, int &i, Value *&value)
{
  int len = str.size();

  if (i >= len) return false;

  char c = str[i];

  if      (c == '\"') {
    std::string str1;

    if (! readString(str, i, str1))
      return false;

    value = new String(str1);
  }
  else if (c == '-' || isdigit(c)) {
    std::string str1;

    if (! readNumber(str, i, str1))
      return false;

    bool ok;

    double n = CJson::stod(str1, ok);

    value = new Number(n);
  }
  else if (c == '{') {
    Object *obj;

    if (! readObject(str, i, obj))
      return false;

    value = obj;
  }
  else if (c == '[') {
    Array *array;

    if (! readArray(str, i, array))
      return false;

    value = array;
  }
  else if (i < len - 3 &&
           str[i] == 't' && str[i + 1] == 'r' && str[i + 2] == 'u' && str[i + 3] == 'e') {
    value = new True;

    i += 4;
  }
  else if (i < len - 4 &&
           str[i + 0] == 'f' && str[i + 1] == 'a' && str[i + 2] == 'l' &&
           str[i + 3] == 's' && str[i + 4] == 'e') {
    value = new False;

    i += 5;
  }
  else if (i < len - 3 &&
           str[i] == 'n' && str[i + 1] == 'u' && str[i + 2] == 'l' && str[i + 3] == 'l') {
    value = new Null;

    i += 4;
  }
  else
    return false;

  return true;
}

bool
CJson::
readLine(FILE *fp, std::string &line)
{
  line = "";

  if (feof(fp)) return false;

  char c = fgetc(fp);

  while (! feof(fp) && c != '\n') {
    line += c;

    c = fgetc(fp);
  }

  return true;
}

// load file and return root value
bool
CJson::
loadFile(const std::string &filename, Value *&value)
{
  FILE *fp = fopen(filename.c_str(), "r");
  if (! fp) return false;

  std::string lines;

  while (! feof(fp)) {
    std::string line;

    if (readLine(fp, line))
      lines += line;
  }

  fclose(fp);

  std::vector<std::string> strs;

  int i   = 0;
  int len = lines.size();

  skipSpace(lines, i);

  if      (i < len && lines[i] == '{') { // object
    Object *obj;

    if (! readObject(lines, i, obj))
      return false;

    value = obj;
  }
  else if (i < len && lines[i] == '[') { // array
    Array *array;

    if (! readArray(lines, i, array))
      return false;

    value = array;
  }
  else
    return false;

  return true;
}

//------

bool
CJson::
matchObject(Value *value, const std::string &match, Value* &value1)
{
  if (isDebug())
    std::cerr << "matchObject \'" << match << "\'" << std::endl;

  if (! value->isObject()) {
    if (! isQuiet())
      std::cerr << value->typeName() << " is not an object" << std::endl;
    return false;
  }

  Object *obj = value->cast<Object>();

  if (match == "?" || match == "?keys") {
    std::vector<std::string> names;

    obj->getNames(names);

    Array *array = new Array;

    for (const auto &n : names) {
      String *str = new String(n);

      array->addValue(str);
    }

    value1 = array;
  }
  else if (match == "?type") {
    String *str = new String(obj->typeName());

    value1 = str;
  }
  else if (match == "?values") {
    std::vector<Value *> values;

    obj->getValues(values);

    Array *array = new Array;

    for (const auto &v : values)
      array->addValue(v);

   value1 = array;
  }
  else {
    if (! obj->getNamedValue(match, value1)) {
      if (! isQuiet())
        std::cerr << "no value \'" << match << "\'" << std::endl;
      return false;
    }
  }

  return true;
}

bool
CJson::
matchArray(Value *value, const std::string &lhs, const std::string &rhs, Array::Values &values)
{
  if (isDebug())
    std::cerr << "matchArray \'" << lhs << "\' \'" << rhs << "\'" << std::endl;

  if (! value->isArray()) {
    if (! isQuiet())
      std::cerr << value->typeName() << " is not an array" << std::endl;
    return false;
  }

  Array *array = value->cast<Array>();

  if (lhs[0] != '[' || lhs[lhs.size() - 1] != ']')
    return false;

  std::string range = lhs.substr(1, lhs.size() - 2);

  if (range == "?size") {
    Number *n = new Number(array->size());

    values.push_back(n);

    return true;
  }

  auto p = range.find(',');

  if (p != std::string::npos) {
    std::string match1 = range;

    std::string lhs1 = match1.substr(0, p);
    std::string rhs1 = match1.substr(p + 1);

    bool ok1, ok2;

    int i1 = CJson::stol(lhs1, ok1);
    int i2 = CJson::stol(rhs1, ok2);

    if (! ok1 || ! ok2) {
      if (! isQuiet())
        std::cerr << "Invalid array indices '" << lhs1 << "', '" << rhs1 << "'" << std::endl;
      return false;
    }

    for (int i = i1; i <= i2 && i < int(array->size()); ++i) {
      Value *value1 = array->at(i);

      if (rhs1 != "")
        matchValues(value1, i, rhs1, values);
      else
        values.push_back(value1);
    }
  }
  else if (range != "") {
    bool ok;

    int i1 = CJson::stol(range, ok);

    if (! ok) {
      if (! isQuiet())
        std::cerr << "Invalid array index '" << lhs << "'" << std::endl;
      return false;
    }

    int i = 0;

    for (const auto &v : array->values()) {
      if (i == i1) {
        if (rhs != "")
          matchValues(v, i, rhs, values);
        else
          values.push_back(v);
      }

      ++i;
    }
  }
  else {
    int i = 0;

    for (const auto &v : array->values()) {
      if (rhs != "")
        matchValues(v, i, rhs, values);
      else
        values.push_back(v);

      ++i;
    }
  }

  return true;
}

bool
CJson::
matchList(Value *value, int ind, const std::string &lhs,
          const std::string &rhs, Array::Values &values)
{
  if (isDebug())
    std::cerr << "matchList \'" << lhs << "\' \'" << rhs << "\'" << std::endl;

  if (lhs[0] != '{' || lhs[lhs.size() - 1] != '}')
    return false;

  std::string names = lhs.substr(1, lhs.size() - 2);

  std::vector<std::string> fields;

  auto p = names.find(",");

  while (p != std::string::npos) {
    std::string name = names.substr(0, p);

    names = names.substr(p + 1);

    fields.push_back(name);

    p = names.find(",");
  }

  fields.push_back(names);

  Array *array = new Array;

  for (const auto &f : fields) {
    Array::Values values1;

    std::string match = (rhs != "" ? f + "/" + rhs : f);

    matchValues(value, ind, match, values1);

    for (const auto &v1 : values1)
      array->addValue(v1);
  }

  values.push_back(array);

  return true;
}

bool
CJson::
matchValues(Value *value, int ind, const std::string &match, Array::Values &values)
{
  std::string match1 = match;

  if (match1 != "" && match1[0] != '{') {
    auto p = match1.find("/");

    while (p != std::string::npos) {
      std::string lhs = match1.substr(0, p);
      std::string rhs = match1.substr(p + 1);

      if (lhs == "")
        return false;

      if      (lhs[0] == '[') {
        return matchArray(value, lhs, rhs, values);
      }
      else if (lhs[0] == '{') {
        return matchList(value, ind, lhs, rhs, values);
      }
      else {
        Value *value1 = 0;

        if (! matchObject(value, lhs, value1))
          return false;

        value = value1;
      }

      match1 = rhs;

      p = match1.find("/");
    }
  }

  if (match1 == "")
    return true;

  if      (match1[0] == '[') {
    return matchArray(value, match1, "", values);
  }
  else if (match1[0] == '{') {
    return matchList(value, ind, match1, "", values);
  }
  else if (match1[0] == '#') {
    int base = 0;

    if (match1.size() > 1) {
      bool ok;

      base = CJson::stol(match1.substr(1), ok);
    }

    Number *n = new Number(base + ind);

    values.push_back(n);
  }
  else {
    Value *value1 = 0;

    if (! matchObject(value, match1, value1))
      return false;

    value = value1;

    if (value)
      values.push_back(value);
  }

  return true;
}

//------

bool
CJson::String::
toReal(double &r) const
{
  bool ok;

  r = CJson::stod(str_, ok);

  return ok;
}

void
CJson::String::
printReal(std::ostream &os) const
{
  double r;

  if (toReal(r))
    os << r;
  else
    print(os);
}

void
CJson::String::
print(std::ostream &os) const
{
  os << "\"" << str_ << "\"";
}

//------

void
CJson::Number::
print(std::ostream &os) const
{
  os << value_;
}

//------

void
CJson::True::
print(std::ostream &os) const
{
  os << "\"true\"";
}

//------

void
CJson::False::
print(std::ostream &os) const
{
  os << "\"false\"";
}

//------

void
CJson::Null::
print(std::ostream &os) const
{
  os << "\"null\"";
}

//------

void
CJson::Object::
printReal(std::ostream &os) const
{
  bool first = true;

  if (! isPrintFlat())
    os << "{";

  for (const auto &nv : nameValueArray_) {
    if (! isPrintFlat()) {
      if (! first) os << ",";
    }
    else {
      if (! first) os << " ";
    }

    os << "\"" << nv.first << "\":";

    nv.second->printReal(os);

    first = false;
  }

  if (! isPrintFlat())
    os << "}";
}

void
CJson::Object::
print(std::ostream &os) const
{
  bool first = true;

  if (! isPrintFlat())
    os << "{";

  for (const auto &nv : nameValueArray_) {
    if (! isPrintFlat()) {
      if (! first) os << ",";
    }
    else {
      if (! first) os << " ";
    }

    os << "\"" << nv.first << "\":";

    nv.second->print(os);

    first = false;
  }

  if (! isPrintFlat())
    os << "}";
}

//------

void
CJson::Array::
printReal(std::ostream &os) const
{
  bool first = true;

  if (! isPrintFlat())
    os << "[";

  for (const auto &v : values_) {
    if (! isPrintFlat()) {
      if (! first) os << ",";
    }
    else {
      if (! first) os << " ";
    }

    v->printReal(os);

    first = false;
  }

  if (! isPrintFlat())
    os << "]";
}
//------

void
CJson::Array::
print(std::ostream &os) const
{
  bool first = true;

  if (! isPrintFlat())
    os << "[";

  for (const auto &v : values_) {
    if (! isPrintFlat()) {
      if (! first) os << ",";
    }
    else {
      if (! first) os << " ";
    }

    v->print(os);

    first = false;
  }

  if (! isPrintFlat())
    os << "]";
}
