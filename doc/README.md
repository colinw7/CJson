+ Extract Information fron JSON files
  + Match Syntax
    +  fields are separated by slash '/'
    +  values can be grouped using braces {<match>,<match>,...}
    +  arrays are added using square brackets with optional index range [<start>:<end>]
    +  list of object keys can be returned using ? or ?keys
    +  list of object values can be returned using ?values
    +  object type can be returned using ?type
    +  array index can be added using #
  + Match Example
    +  e.g. "head/[1:3]/{name1,name2}/?
