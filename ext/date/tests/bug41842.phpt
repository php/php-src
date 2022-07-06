--TEST--
Bug #41842 (Cannot create years < 0100 & negative years with date_create or new DateTime)
--FILE--
<?php
date_default_timezone_set("UTC");

$date = new DateTime('-2007-06-28 00:00:00');
echo $date->format(DATE_ISO8601);
?>
--EXPECT--
-2007-06-28T00:00:00+0000
