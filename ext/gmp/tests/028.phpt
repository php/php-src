--TEST--
gmp_random() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_strval(gmp_random()));
var_dump(gmp_strval(gmp_random(-1)));
var_dump(gmp_strval(gmp_random(0)));
var_dump(gmp_strval(gmp_random(10)));
var_dump(gmp_strval(gmp_random("-10")));
var_dump(gmp_strval(gmp_random(-10)));

var_dump(gmp_random(array()));
var_dump(gmp_random(array(),1));
var_dump(gmp_random(""));
var_dump(gmp_random("test"));

echo "Done\n";
?>
--EXPECTF--	
string(%d) "%d"
string(%d) "%d"
string(1) "%d"
string(%d) "%d"
string(%d) "%d"
string(%d) "%d"

Warning: gmp_random() expects parameter 1 to be long, array given in %s on line %d
NULL

Warning: gmp_random() expects at most 1 parameter, 2 given in %s on line %d
NULL

Warning: gmp_random() expects parameter 1 to be long, string given in %s on line %d
NULL

Warning: gmp_random() expects parameter 1 to be long, string given in %s on line %d
NULL
Done
