--TEST--
Test function getservbyname() by calling it more than or less than its expected arguments
--CREDITS--
Italian PHP TestFest 2009 Cesena 19-20-21 june
Fabio Fabbrucci (fabbrucci@grupporetina.com)
Michele Orselli (mo@ideato.it)
Danilo Sanchi (sanchi@grupporetina.com)
--FILE--
<?php
$service = "www";
$protocol = "tcp";
$extra_arg = 12;
var_dump(getservbyname($service, $protocol, $extra_arg ) );
var_dump(getservbyname($service));
?>
--EXPECTF--
Warning: getservbyname() expects exactly 2 parameters, %d given in %s on line %d
NULL

Warning: getservbyname() expects exactly 2 parameters, %d given in %s on line %d
NULL
