--TEST--
Test OnUpdateStr and OnUpdateStrNotEmpty validators.
--EXTENSIONS--
zend_test
--FILE--
<?php
echo 'str_test INI', PHP_EOL;
var_dump(ini_get("zend_test.str_test"));
var_dump(ini_set("zend_test.str_test", "Test"));
var_dump(ini_get("zend_test.str_test"));
var_dump(ini_set("zend_test.str_test", ""));
var_dump(ini_get("zend_test.str_test"));

echo 'not_empty_str_test INI', PHP_EOL;
var_dump(ini_get("zend_test.not_empty_str_test"));
var_dump(ini_set("zend_test.not_empty_str_test", "Test"));
var_dump(ini_get("zend_test.not_empty_str_test"));
var_dump(ini_set("zend_test.not_empty_str_test", ""));
var_dump(ini_get("zend_test.not_empty_str_test"));
?>
--EXPECT--
str_test INI
string(0) ""
string(0) ""
string(4) "Test"
string(4) "Test"
string(0) ""
not_empty_str_test INI
string(3) "val"
string(3) "val"
string(4) "Test"
bool(false)
string(4) "Test"
