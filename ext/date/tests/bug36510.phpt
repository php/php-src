--TEST--
Bug #36510 (strtotime() fails to parse date strings with tabs)
--FILE--
<?php
date_default_timezone_set("UTC");

$t = 1140973388;

var_dump(strtotime("-2 hours", $t));
var_dump(strtotime("-2\thours", $t));
?>
--EXPECT--
int(1140966188)
int(1140966188)
