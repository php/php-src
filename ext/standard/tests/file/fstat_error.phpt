--TEST--
Test function fstat() by calling it more than or less than its expected arguments
--FILE--
<?php
$fp = fopen (__FILE__, 'r');
$extra_arg = 'nothing'; 

var_dump(fstat( $fp, $extra_arg ) );
var_dump(fstat());

fclose($fp);

?>
===DONE===
--EXPECTF--

Warning: fstat() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

Warning: fstat() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
===DONE===