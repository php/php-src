--TEST--
json_validate() - General usage
--FILE--
<?php

var_dump(
  json_validate(""),
  json_validate("."),
  json_validate("<?>"),
  json_validate(";"),
  json_validate("руссиш"),
  json_validate("blah"),
  json_validate('{ "": "": "" } }'),
  json_validate('{ "": { "": "" }'),
  json_validate('{ "test": {} "foo": "bar" }, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'),

  json_validate('{ "test": { "foo": "bar" } }'),
  json_validate('{ "test": { "foo": "" } }'),
  json_validate('{ "": { "foo": "" } }'),
  json_validate('{ "": { "": "" } }'),
  json_validate('{ "test": {"foo": "bar"}, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'),
  json_validate('{ "test": {"foo": "bar"}, "test2": {"foo" : "bar" }, "test3": {"foo" : "bar" } }'),
);

?>
--EXPECT--
bool(false)
bool(false)
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
bool(true)
bool(true)
