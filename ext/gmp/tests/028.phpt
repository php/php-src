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
resource(%d) of type (GMP integer)

Warning: Wrong parameter count for gmp_random() in %s on line %d
NULL
resource(%d) of type (GMP integer)
resource(%d) of type (GMP integer)
Done
