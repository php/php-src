--TEST--
Specifying non-default syntax in mb_ereg_search()
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_ereg_search")) die("skip mb_ereg_search() is not defined");
?>
--FILE--
<?php

mb_ereg_search_init("a");
var_dump(mb_ereg_search("a\\{1,2\\}", "b"));

?>
--EXPECT--
bool(true)
