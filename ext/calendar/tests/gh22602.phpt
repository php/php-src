--TEST--
Bug GH-22602: (gregoriantojd() integer overflow with INT_MAX year)
--EXTENSIONS--
calendar
--FILE--
<?php
var_dump(gregoriantojd(5, 5, PHP_INT_MAX > 2147483647 ? 2147483647 : PHP_INT_MAX));
?>
--EXPECTF--
int(%d)
