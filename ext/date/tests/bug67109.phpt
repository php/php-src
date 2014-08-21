--TEST--
Bug #67109 (First uppercase letter breaks date string parsing)
--INI--
date.timezone=UTC
--FILE--
<?php
var_dump(date('d.m.Y',strtotime('last day of april')));
var_dump(date('d.m.Y',strtotime('Last day of april')));
var_dump(date('d.m.Y',strtotime('lAst Day of April')));
?>
--EXPECTF--
string(10) "30.04.%d"
string(10) "30.04.%d"
string(10) "30.04.%d"
