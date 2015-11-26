--TEST--
Bug #35705 (strtotime() fails to parse soap date format without TZ)
--FILE--
<?php
date_default_timezone_set("UTC");

echo date(DATE_ISO8601, strtotime('2000-10-10T10:12:30.000')) . "\n";

?>
--EXPECT--
2000-10-10T10:12:30+0000
