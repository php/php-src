--TEST--
Bug #76999 (mb_regex_set_options() return current options)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
if (!function_exists('mb_regex_set_options')) die('skip mb_regex_set_options() not available');
?>
--FILE--
<?php
mb_regex_set_options("pr");
var_dump(mb_regex_set_options("m"));
var_dump(mb_regex_set_options("mdi"));
var_dump(mb_regex_set_options("m"));
try {
    var_dump(mb_regex_set_options("a"));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(mb_regex_set_options());
?>
--EXPECT--
string(2) "pr"
string(2) "mr"
string(3) "imd"
Option "a" is not supported
string(2) "mr"
