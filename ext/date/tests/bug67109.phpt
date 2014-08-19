--TEST--
Bug #67109 (First uppercase letter breaks date string parsing)
--FILE--
<?php
var_dump(date('d.m.Y',strtotime('last day of this month')));
var_dump(date('d.m.Y',strtotime('Last day of this month')));
var_dump(date('d.m.Y',strtotime('lAst Day of This Month')));
?>
--EXPECT--
string(10) "31.08.2014"
string(10) "31.08.2014"
string(10) "31.08.2014"
