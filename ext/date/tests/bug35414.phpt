--TEST--
Bug #35414 (strtotime() no longer works with ordinal suffix)
--FILE--
<?php
date_default_timezone_set("UTC");

echo date(DATE_ISO8601, strtotime("Sat 26th Nov 2005 18:18")) . "\n";
echo date(DATE_ISO8601, strtotime("26th Nov", 1134340285)) . "\n";
echo date(DATE_ISO8601, strtotime("Dec. 4th, 2005")) . "\n";
echo date(DATE_ISO8601, strtotime("December 4th, 2005")) . "\n";
?>
--EXPECT--
2005-11-26T18:18:00+0000
2005-11-26T00:00:00+0000
2005-12-04T00:00:00+0000
2005-12-04T00:00:00+0000
