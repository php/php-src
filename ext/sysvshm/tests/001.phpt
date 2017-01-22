--TEST--
ftok() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (!function_exists('ftok')){ print 'skip'; }
?>
--FILE--
<?php

var_dump(ftok());
var_dump(ftok(1));
var_dump(ftok(1,1,1));

var_dump(ftok("",""));
var_dump(ftok(-1, -1));
var_dump(ftok("qwertyu","qwertyu"));

var_dump(ftok("nonexistentfile","q"));

var_dump(ftok(__FILE__,"q"));

echo "Done\n";
?>
--EXPECTF--	
Warning: ftok() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: ftok() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: ftok() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: ftok(): Pathname is invalid in %s on line %d
int(-1)

Warning: ftok(): Project identifier is invalid in %s on line %d
int(-1)

Warning: ftok(): Project identifier is invalid in %s on line %d
int(-1)

Warning: ftok(): ftok() failed - No such file or directory in %s on line %d
int(-1)
int(%d)
Done
