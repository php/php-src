--TEST--
Bug #76374 (Date difference varies according day time)
--FILE--
<?php
date_default_timezone_set('Europe/Paris');

$objDateTo = new dateTime('2017-10-01');
$objDateFrom = new dateTime('2017-01-01');
$interval = $objDateTo->diff($objDateFrom);
echo $interval->m, "\n";

$objDateTo = new dateTime('2017-10-01 12:00:00');
$objDateFrom = new dateTime('2017-01-01 12:00:00');
$interval = $objDateTo->diff($objDateFrom);
echo $interval->m, "\n";
?>
--EXPECT--
9
9
