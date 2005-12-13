--TEST--
Bug #35630 (strtotime() crashes on non-separated relative modifiers)
--FILE--
<?php
date_default_timezone_set("UTC");

echo date(DATE_ISO8601, strtotime('5 january 2006+3day+1day')) . "\n";

?>
--EXPECT--
2006-01-09T00:00:00+0000
