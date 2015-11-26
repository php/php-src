--TEST--
Test function key_exists() by calling it more than or less than its expected arguments
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php

echo "*** Test by calling method or function with incorrect numbers of arguments ***\n";

$a = array('bar' => 1);  
var_dump(key_exists());
var_dump(key_exists('foo', $a, 'baz'));

?>
--EXPECTF--
*** Test by calling method or function with incorrect numbers of arguments ***

Warning: key_exists() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: key_exists() expects exactly 2 parameters, 3 given in %s on line %d
NULL
