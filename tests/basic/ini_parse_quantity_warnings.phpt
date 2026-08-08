--TEST--
ini_parse_quantity() function - warns when given inappropriate values
--INI--
error_reporting = E_ALL
--FILE--
<?php
ini_parse_quantity('-1');
ini_parse_quantity('1mb');
ini_parse_quantity('256 then skip a few then g')
?>
--EXPECTF--
Warning: ini_parse_quantity(): Invalid quantity "1mb": unknown multiplier "b", interpreting as "1" for backwards compatibility in %s on line %d

Warning: ini_parse_quantity(): Invalid quantity "256 then skip a few then g", interpreting as "256 g" for backwards compatibility in %s on line %d
