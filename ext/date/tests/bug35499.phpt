--TEST--
Bug #35499 (strtotime() does not handle whitespace around the date string)
--FILE--
<?php
date_default_timezone_set("UTC");

echo date(DATE_ISO8601, strtotime("11/20/2005 8:00 AM \r\n")) . "\n";
echo date(DATE_ISO8601, strtotime("  11/20/2005 8:00 AM \r\n")) . "\n";
var_dump(strtotime(" a "));
var_dump(strtotime(" \n "));
?>
--EXPECT--
2005-11-20T08:00:00+0000
2005-11-20T08:00:00+0000
bool(false)
bool(false)
