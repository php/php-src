--TEST--
Bug #35456 (+ 1 [time unit] format did not work)
--FILE--
<?php
date_default_timezone_set("UTC");

$t = 1133216119;

echo date(DATE_ISO8601, strtotime("+ 1 day", $t)) . "\n";
echo date(DATE_ISO8601, strtotime("+ 1 month", $t)) . "\n";
echo date(DATE_ISO8601, strtotime("+ 1 week", $t)) . "\n";
?>
--EXPECT--
2005-11-29T22:15:19+0000
2005-12-28T22:15:19+0000
2005-12-05T22:15:19+0000
