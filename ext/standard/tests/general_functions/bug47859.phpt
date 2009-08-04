--TEST--
Bug #47859 (parse_ini_file() does not like asterisk (*) in key in the beginning)
--FILE--
<?php
var_dump(parse_ini_string('*key = "*value"'));
var_dump(parse_ini_string('-key = "-value"'));
var_dump(parse_ini_string('_key = "_value"'));

var_dump(parse_ini_string('key* = "value*"'));
var_dump(parse_ini_string('key.*.* = "value.*.*"'));
var_dump(parse_ini_string('*.*.key = "*.*.value"'));
var_dump(parse_ini_string('k*e*y = "v*a*lue"'));
?>
--EXPECT--
array(1) {
  [u"*key"]=>
  unicode(6) "*value"
}
array(1) {
  [u"-key"]=>
  unicode(6) "-value"
}
array(1) {
  [u"_key"]=>
  unicode(6) "_value"
}
array(1) {
  [u"key*"]=>
  unicode(6) "value*"
}
array(1) {
  [u"key.*.*"]=>
  unicode(9) "value.*.*"
}
array(1) {
  [u"*.*.key"]=>
  unicode(9) "*.*.value"
}
array(1) {
  [u"k*e*y"]=>
  unicode(7) "v*a*lue"
}
