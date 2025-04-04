--TEST--
json_validate() - General usage
--FILE--
<?php

var_dump(
  json_validate('{"key":"'.str_repeat('x', 1000).'"foobar}'),
  json_last_error_msg(),
  json_validate(""),
  json_last_error_msg(),
  json_validate("."),
  json_last_error_msg(),
  json_validate("<?>"),
  json_last_error_msg(),
  json_validate(";"),
  json_last_error_msg(),
  json_validate("руссиш"),
  json_last_error_msg(),
  json_validate("blah"),
  json_last_error_msg(),
  json_validate('{ "": "": "" } }'),
  json_last_error_msg(),
  json_validate('{ "": { "": "" }'),
  json_last_error_msg(),
  json_validate('{ "test": {} "foo": "bar" }, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'),
  json_last_error_msg(),
  json_validate('{ "test": { "foo": "bar" } }'),
  json_last_error_msg(),
  json_validate('{ "test": { "foo": "" } }'),
  json_last_error_msg(),
  json_validate('{ "": { "foo": "" } }'),
  json_last_error_msg(),
  json_validate('{ "": { "": "" } }'),
  json_last_error_msg(),
  json_validate('{ "test": {"foo": "bar"}, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'),
  json_last_error_msg(),
  json_validate('{ "test": {"foo": "bar"}, "test2": {"foo" : "bar" }, "test3": {"foo" : "bar" } }'),
  json_last_error_msg(),
);

?>
--EXPECT--
bool(false)
string(53) "Syntax error, at character 1009 near content: foobar}"
bool(false)
string(12) "Syntax error"
bool(false)
string(44) "Syntax error, at character 0 near content: ."
bool(false)
string(46) "Syntax error, at character 0 near content: <?>"
bool(false)
string(44) "Syntax error, at character 0 near content: ;"
bool(false)
string(55) "Syntax error, at character 0 near content: руссиш"
bool(false)
string(47) "Syntax error, at character 0 near content: blah"
bool(false)
string(51) "Syntax error, at character 8 near content: : "" } }"
bool(false)
string(12) "Syntax error"
bool(false)
string(75) "Syntax error, at character 13 near content: "foo": "bar" }, "test2": {"foo""
bool(true)
string(8) "No error"
bool(true)
string(8) "No error"
bool(true)
string(8) "No error"
bool(true)
string(8) "No error"
bool(true)
string(8) "No error"
bool(true)
string(8) "No error"
