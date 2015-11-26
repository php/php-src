--TEST--
Bug #41844 (Format returns incorrect number of digits for negative years -0001 to -0999)
--FILE--
<?php
date_default_timezone_set("UTC");

$date = new DateTime('2007-06-28');
$date->modify('-3006 years');
echo $date->format(DATE_ISO8601), "\n";

$date = new DateTime('2007-06-28');
$date->modify('-2008 years');
echo $date->format(DATE_ISO8601), "\n";
?>
--EXPECT--
-0999-06-28T00:00:00+0000
-0001-06-28T00:00:00+0000
