--TEST--
str_first_char() function
--SKIPIF--
<?php
if (!function_exists('str_first_char')) {
    die('skip str_first_char() is not available');
}
?>
--FILE--
<?php
var_dump(str_first_char("hello"));
var_dump(str_first_char("123"));
var_dump(str_first_char(""));
var_dump(str_first_char(" a b c "));
?>
--EXPECT--
string(1) "h"
string(1) "1"
string(0) ""
string(1) " "
