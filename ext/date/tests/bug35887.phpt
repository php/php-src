--TEST--
Bug #35887 (wddx_deserialize not parsing dateTime fields properly)
--FILE--
<?php
date_default_timezone_set("UTC");

echo date(DATE_ISO8601, strtotime('2006-1-6T0:0:0-8:0')) . "\n";

?>
--EXPECT--
2006-01-06T08:00:00+0000
