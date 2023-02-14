--TEST--
Bug #35885 (strtotime("NOW") no longer works)
--FILE--
<?php
date_default_timezone_set("UTC");

$time = time();
$ts = date(DATE_ATOM, strtotime('NOW', $time));
$ts2 = date(DATE_ATOM, $time);

$res = ($ts == $ts2);
var_dump($res);

if (!$res) {
    var_dump($ts);
    var_dump($ts2);
}

?>
--EXPECT--
bool(true)
