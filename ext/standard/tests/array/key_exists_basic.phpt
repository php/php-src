--TEST--
Test function key_exists() by calling it with its expected arguments
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php
echo "*** test key_exists() by calling it with its expected arguments ***\n";
$a = array('bar' => 1);
var_dump(key_exists('bar', $a));
var_dump(key_exists('foo', $a));
--EXPECTF--
*** test key_exists() by calling it with its expected arguments ***
bool(true)
bool(false)
