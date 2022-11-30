--TEST--
Bug GH-9880 (DateTime diff returns wrong sign on day count when using a timezone)
--FILE--
<?php

ini_set('date.timezone', 'America/Los_Angeles');

$nowTime = new DateTime();
$nowTime->setTimestamp(1667416695);

$dateTime = new DateTime();
$dateTime->setTimestamp(1671904800);
$dateTime->setTimezone(new DateTimeZone('America/New_York'));

echo $dateTime->diff($nowTime)->format('%R %a %H %I %S'), "\n";
?>
--EXPECT--
- 51 22 41 45
