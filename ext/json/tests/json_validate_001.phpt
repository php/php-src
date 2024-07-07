--TEST--
json_validate() - General usage
--FILE--
<?php

var_dump(
  json_validate('{"key":"'.str_repeat('x', 1000).'"foobar}'),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate(""),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate("."),
  json_last_error_msg(),
  json_last_error_info(),  
  json_validate("<?>"),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate(";"),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate("руссиш"),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate("blah"),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate('{ "": "": "" } }'),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate('{ "": { "": "" }'),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate('{ "test": {} "foo": "bar" }, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'),
  json_last_error_msg(),
  json_last_error_info(),

  json_validate('{ "test": { "foo": "bar" } }'),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate('{ "test": { "foo": "" } }'),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate('{ "": { "foo": "" } }'),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate('{ "": { "": "" } }'),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate('{ "test": {"foo": "bar"}, "test2": {"foo" : "bar" }, "test2": {"foo" : "bar" } }'),
  json_last_error_msg(),
  json_last_error_info(),
  json_validate('{ "test": {"foo": "bar"}, "test2": {"foo" : "bar" }, "test3": {"foo" : "bar" } }'),
  json_last_error_msg(),
  json_last_error_info(),
);

?>
--EXPECT--
bool(false)
string(53) "Syntax error, at character 1009 near content: foobar}"
array(5) {
  ["error_code"]=>
  int(4)
  ["error_msg"]=>
  string(12) "Syntax error"
  ["error_position"]=>
  int(1009)
  ["total_input_length"]=>
  int(1016)
  ["at_content"]=>
  string(7) "foobar}"
}
bool(false)
string(12) "Syntax error"
array(5) {
  ["error_code"]=>
  int(0)
  ["error_msg"]=>
  string(12) "Syntax error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(0)
  ["at_content"]=>
  string(0) ""
}
bool(false)
string(44) "Syntax error, at character 0 near content: ."
array(5) {
  ["error_code"]=>
  int(4)
  ["error_msg"]=>
  string(12) "Syntax error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(1)
  ["at_content"]=>
  string(1) "."
}
bool(false)
string(46) "Syntax error, at character 0 near content: <?>"
array(5) {
  ["error_code"]=>
  int(4)
  ["error_msg"]=>
  string(12) "Syntax error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(3)
  ["at_content"]=>
  string(3) "<?>"
}
bool(false)
string(44) "Syntax error, at character 0 near content: ;"
array(5) {
  ["error_code"]=>
  int(4)
  ["error_msg"]=>
  string(12) "Syntax error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(1)
  ["at_content"]=>
  string(1) ";"
}
bool(false)
string(55) "Syntax error, at character 0 near content: руссиш"
array(5) {
  ["error_code"]=>
  int(4)
  ["error_msg"]=>
  string(12) "Syntax error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(12)
  ["at_content"]=>
  string(12) "руссиш"
}
bool(false)
string(47) "Syntax error, at character 0 near content: blah"
array(5) {
  ["error_code"]=>
  int(4)
  ["error_msg"]=>
  string(12) "Syntax error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(4)
  ["at_content"]=>
  string(4) "blah"
}
bool(false)
string(51) "Syntax error, at character 8 near content: : "" } }"
array(5) {
  ["error_code"]=>
  int(4)
  ["error_msg"]=>
  string(12) "Syntax error"
  ["error_position"]=>
  int(8)
  ["total_input_length"]=>
  int(16)
  ["at_content"]=>
  string(8) ": "" } }"
}
bool(false)
string(12) "Syntax error"
array(5) {
  ["error_code"]=>
  int(4)
  ["error_msg"]=>
  string(12) "Syntax error"
  ["error_position"]=>
  int(16)
  ["total_input_length"]=>
  int(16)
  ["at_content"]=>
  string(0) ""
}
bool(false)
string(75) "Syntax error, at character 13 near content: "foo": "bar" }, "test2": {"foo""
array(5) {
  ["error_code"]=>
  int(4)
  ["error_msg"]=>
  string(12) "Syntax error"
  ["error_position"]=>
  int(13)
  ["total_input_length"]=>
  int(83)
  ["at_content"]=>
  string(31) ""foo": "bar" }, "test2": {"foo""
}
bool(true)
string(8) "No error"
array(5) {
  ["error_code"]=>
  int(0)
  ["error_msg"]=>
  string(8) "No error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(0)
  ["at_content"]=>
  string(0) ""
}
bool(true)
string(8) "No error"
array(5) {
  ["error_code"]=>
  int(0)
  ["error_msg"]=>
  string(8) "No error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(0)
  ["at_content"]=>
  string(0) ""
}
bool(true)
string(8) "No error"
array(5) {
  ["error_code"]=>
  int(0)
  ["error_msg"]=>
  string(8) "No error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(0)
  ["at_content"]=>
  string(0) ""
}
bool(true)
string(8) "No error"
array(5) {
  ["error_code"]=>
  int(0)
  ["error_msg"]=>
  string(8) "No error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(0)
  ["at_content"]=>
  string(0) ""
}
bool(true)
string(8) "No error"
array(5) {
  ["error_code"]=>
  int(0)
  ["error_msg"]=>
  string(8) "No error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(0)
  ["at_content"]=>
  string(0) ""
}
bool(true)
string(8) "No error"
array(5) {
  ["error_code"]=>
  int(0)
  ["error_msg"]=>
  string(8) "No error"
  ["error_position"]=>
  int(0)
  ["total_input_length"]=>
  int(0)
  ["at_content"]=>
  string(0) ""
}