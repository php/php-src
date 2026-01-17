--TEST--
DateTime::diff() with PHP_INT_MIN timestamp and timezone should not crash
--FILE--
<?php
$nowTime = new DateTime();
$nowTime->setTimestamp(PHP_INT_MIN);

$dateTime = new DateTime();
$dateTime->setTimezone(new DateTimeZone('America/New_York'));

echo $dateTime->diff($nowTime)->format('%R %a %H %I %S'), "\n";
?>
--EXPECTREGEX--
^[+-] [0-9]+ [0-9]+ [0-9]+ [0-9]+$