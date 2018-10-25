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
Deprecated: Function gmp_random() is deprecated in %s on line %d
string(%d) "%d"

Deprecated: Function gmp_random() is deprecated in %s on line %d
string(%d) "%d"

Deprecated: Function gmp_random() is deprecated in %s on line %d
string(1) "%d"

Deprecated: Function gmp_random() is deprecated in %s on line %d
string(%d) "%d"

Deprecated: Function gmp_random() is deprecated in %s on line %d
string(%d) "%d"

Deprecated: Function gmp_random() is deprecated in %s on line %d
string(%d) "%d"

Deprecated: Function gmp_random() is deprecated in %s on line %d

Warning: gmp_random() expects parameter 1 to be int, array given in %s on line %d
NULL

Deprecated: Function gmp_random() is deprecated in %s on line %d

Warning: gmp_random() expects at most 1 parameter, 2 given in %s on line %d
NULL

Deprecated: Function gmp_random() is deprecated in %s on line %d

Warning: gmp_random() expects parameter 1 to be int, string given in %s on line %d
NULL

Deprecated: Function gmp_random() is deprecated in %s on line %d

Warning: gmp_random() expects parameter 1 to be int, string given in %s on line %d
NULL
Done
