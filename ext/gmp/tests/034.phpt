--TEST--
gmp_clrbit() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

$n = gmp_init(0);
gmp_clrbit($n, 0);
var_dump(gmp_strval($n));

$n = gmp_init(-1);
gmp_clrbit($n, -1);
var_dump(gmp_strval($n));

$n = gmp_init("1000000");
gmp_clrbit($n, -1);
var_dump(gmp_strval($n));

$n = gmp_init("1000000");
gmp_clrbit($n, 3);
var_dump(gmp_strval($n));

$n = gmp_init("238462734628347239571823641234");
gmp_clrbit($n, 3);
gmp_clrbit($n, 5);
gmp_clrbit($n, 20);
var_dump(gmp_strval($n));

$n = array();
gmp_clrbit($n, 3);
gmp_clrbit($n, 3, 1);
gmp_clrbit($n);
gmp_clrbit();

echo "Done\n";
?>
--EXPECTF--	
string(1) "0"

Warning: gmp_clrbit(): Index must be greater than or equal to zero in %s on line %d
string(2) "-1"

Warning: gmp_clrbit(): Index must be greater than or equal to zero in %s on line %d
string(7) "1000000"
string(7) "1000000"
string(30) "238462734628347239571822592658"

Warning: gmp_clrbit() expects parameter 1 to be GMP, array given in %s on line %d

Warning: gmp_clrbit() expects exactly 2 parameters, 3 given in %s on line %d

Warning: gmp_clrbit() expects exactly 2 parameters, 1 given in %s on line %d

Warning: gmp_clrbit() expects exactly 2 parameters, 0 given in %s on line %d
Done
