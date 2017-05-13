--TEST--
Test idn_to_ascii function : wrong parameter (array)
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--FILE--
<?php
$wrong_parameter = array();
$i = idn_to_ascii($wrong_parameter);
?>
--EXPECTF--
Warning: idn_to_ascii() expects parameter 1 to be string, array given in %s on line %d
