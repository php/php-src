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

Warning: Wrong parameter count for fstat() in %s on line %d
NULL

Warning: Wrong parameter count for fstat() in %s on line %d
NULL
===DONE===