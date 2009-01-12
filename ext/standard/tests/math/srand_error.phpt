--TEST--
Test srand() function :  error conditions - incorrect number of args
--FILE--
<?php
/* Prototype  : void srand  ([ int $seed  ] )
 * Description: Seed the random number generator.
 * Source code: ext/standard/rand.c
 */
 
/*
 * Pass incorrect number of arguments to srand() to test behaviour
 */
 
echo "*** Testing srand() : error conditions ***\n";

var_dump(srand(500, true));
var_dump(srand("fivehundred"));
var_dump(srand("500ABC"));
?>
===Done===
--EXPECTF--
*** Testing srand() : error conditions ***

Warning: srand() expects at most 1 parameter, 2 given in %s on line %d
NULL

Warning: srand() expects parameter 1 to be long, string given in %s on line %d
NULL

Notice: A non well formed numeric value encountered in %s on line %d
NULL
===Done===
