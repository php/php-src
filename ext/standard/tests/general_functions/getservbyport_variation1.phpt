--TEST--
Test function getservbyport() by calling it more than or less than its expected arguments
--DESCRIPTION--
Test function passing invalid port number and invalid protocol name
--CREDITS--
Italian PHP TestFest 2009 Cesena 19-20-21 june
Fabio Fabbrucci (fabbrucci@grupporetina.com)
Michele Orselli (mo@ideato.it)
Simone Gentili (sensorario@gmail.com)
--FILE--
<?php
	var_dump(getservbyport( -1, "tcp" ));
	var_dump(getservbyport( 80, "ppp" ));
	var_dump(getservbyport( null, null));
	var_dump(getservbyport( array(), array()));
	var_dump(getservbyport( array(80), array("tcp")));
	var_dump(getservbyport( array(2, 3), array("one"=>1, "two"=>2)));
	var_dump(getservbyport( 2, 2));
	var_dump(getservbyport( "80", "tcp"));
	var_dump(getservbyport( new stdClass(), new stdClass()));

?>
--EXPECTF--
bool(false)
bool(false)
bool(false)

Warning: getservbyport() expects parameter 1 to be int, array given in %s on line %d
NULL

Warning: getservbyport() expects parameter 1 to be int, array given in %s on line %d
NULL

Warning: getservbyport() expects parameter 1 to be int, array given in %s on line %d
NULL
bool(false)
string(%d) "%s"

Warning: getservbyport() expects parameter 1 to be int, object given in %s on line %d
NULL
