--TEST--
is_json() - General usage
--FILE--
<?php
var_dump(
  is_json(""),
  is_json("."),
  is_json("<?>"),
  is_json(";"),
  is_json("руссиш"),
  is_json("blah"),
  is_json(NULL),
  is_json('{ "test": { "foo": "bar" } }'),
  is_json('{ "test": { "foo": "" } }'),
  is_json('{ "": { "foo": "" } }'),
  is_json('{ "": { "": "" } }'),
  is_json('{ "": { "": "" }'),
  is_json('{ "test": {} "foo": "bar" }, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'),
  is_json('{ "test": {"foo": "bar"}, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'),
  is_json('{ "test": {"foo": "bar"}, "test2": {"foo" : "bar" }, "test3": {"foo" : "bar" } }'),
  is_json('{ "": "": "" } }')
);
?>

--EXPECTF--
Deprecated: is_json(): Passing null to parameter #1 ($json) of type string is deprecated in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
