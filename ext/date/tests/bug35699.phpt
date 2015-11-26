--TEST--
Bug #35699 (date() can't handle leap years before 1970)
--FILE--
<?php
date_default_timezone_set("UTC");

echo date(DATE_ISO8601, strtotime('1964-06-06')), "\n";
echo date(DATE_ISO8601, strtotime('1963-06-06')), "\n";
echo date(DATE_ISO8601, strtotime('1964-01-06')), "\n";
?>
--EXPECT--
1964-06-06T00:00:00+0000
1963-06-06T00:00:00+0000
1964-01-06T00:00:00+0000
