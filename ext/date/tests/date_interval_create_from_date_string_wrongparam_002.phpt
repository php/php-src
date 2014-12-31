--TEST--
Test date_interval_create_from_date_string() function : with 2 parameters (wrong).
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--FILE--
<?php
$i = date_interval_create_from_date_string('1 year', 'wrong');
?>
--EXPECTF--
Warning: date_interval_create_from_date_string() expects exactly 1 parameter, 2 given in %s on line %d