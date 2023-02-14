--TEST--
Bug #35422 (strtotime() does not parse times with UTC as timezone)
--FILE--
<?php
date_default_timezone_set("UTC");

echo date(DATE_ATOM, strtotime("July 1, 2000 00:00:00 UTC")) . "\n";
echo date(DATE_ATOM, strtotime("July 1, 2000 00:00:00 GMT")) . "\n";
?>
--EXPECT--
2000-07-01T00:00:00+00:00
2000-07-01T00:00:00+00:00
