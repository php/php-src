--TEST--
Test date_interval_create_from_date_string() function : basic functionality
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--FILE--
<?php
$string = '1 day'; //P1D
$i = date_interval_create_from_date_string($string);
var_dump($i->d);

$string = '2 weeks'; //14 days
$i = date_interval_create_from_date_string($string);
var_dump($i->d);

$string = '3 months';
$i = date_interval_create_from_date_string($string);
var_dump($i->m);

$string = '4 years';
$i = date_interval_create_from_date_string($string);
var_dump($i->y);

$string = '1 year + 1 day';
$i = date_interval_create_from_date_string($string);
var_dump($i->y);
var_dump($i->d);
?>
--EXPECTF--
int(1)
int(14)
int(3)
int(4)
int(1)
int(1)
