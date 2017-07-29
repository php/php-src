--TEST--
Test date_interval_create_from_date_string() function : wrong parameter (array)
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--FILE--
<?php
$wrong_parameter = array();
$i = date_interval_create_from_date_string($wrong_parameter);
?>
--EXPECTF--
Warning: date_interval_create_from_date_string() expects parameter 1 to be string (or integer, float, boolean or convertible object), array given in %s on line %d
