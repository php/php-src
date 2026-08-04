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
--EXPECTF--
Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)
