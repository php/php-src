--TEST--
Bug #35885 (strtotime("NOW") no longer works)
--FILE--
<?php
date_default_timezone_set("UTC");

$ts = date(DATE_ISO8601, strtotime('NOW'));
$ts2 = date(DATE_ISO8601, time());

var_dump($ts == $ts2);

?>
--EXPECT--
bool(true)
