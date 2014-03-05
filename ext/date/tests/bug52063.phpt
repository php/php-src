--TEST--
Bug #52063 (DateTime constructor's second argument doesn't have a null default value)
--FILE--
<?php
date_default_timezone_set("Europe/Lisbon");
$a = new DateTime("2009-01-01", null);
echo $a->format(DateTime::COOKIE);
echo "\n";
$a = date_create("2009-01-01", null);
echo $a->format(DateTime::COOKIE);
echo "\n";
?>
--EXPECTF--
Thursday, 01-Jan-2009 00:00:00 WET
Thursday, 01-Jan-2009 00:00:00 WET
