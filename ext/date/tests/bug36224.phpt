--TEST--
Bug #36224 (date(DATE_ATOM) gives wrong resulsts)
--FILE--
<?php
date_default_timezone_set("Europe/Oslo");

echo date(DATE_ATOM, strtotime('2006-01-31T19:23:56Z')) . "\n";
echo date(DATE_ATOM, strtotime('2006-01-31T19:23:56')) . "\n";

?>
--EXPECT--
2006-01-31T20:23:56+01:00
2006-01-31T19:23:56+01:00
