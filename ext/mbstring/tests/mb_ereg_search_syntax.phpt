--TEST--
Specifying non-default syntax in mb_ereg_search()
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring not enabled');
if (!function_exists("mb_regex_search")) die("skip mb_regex_search() is not defined");
?>
--FILE--
<?php

mb_ereg_search_init("a");
var_dump(mb_ereg_search("a\\{1,2\\}", "b"));

?>
--EXPECT--
bool(true)
