--TEST--
Bug #76032 (DateTime->diff having issues with leap days for timezones ahead of UTC)
--FILE--
<?php

date_default_timezone_set('UTC');

$d = new DateTime('2008-03-01');
$a = new DateTime('2018-03-01');

var_dump($d->diff($a)->y);

date_default_timezone_set('Europe/Amsterdam');

$d = new DateTime('2008-03-01');
$a = new DateTime('2018-03-01');

var_dump($d->diff($a)->y);
?>
--EXPECT--
int(10)
int(10)
