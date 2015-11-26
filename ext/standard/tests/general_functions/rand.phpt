--TEST--
rand() and mt_rand() tests
--FILE--
<?php

var_dump(mt_rand());
var_dump(mt_rand(-1));
var_dump(mt_rand(-1,1));
var_dump(mt_rand(0,3));

var_dump(rand());
var_dump(rand(-1));
var_dump(rand(-1,1));
var_dump(rand(0,3));

var_dump(srand());
var_dump(srand(-1));
var_dump(srand(array()));

var_dump(mt_srand());
var_dump(mt_srand(-1));
var_dump(mt_srand(array()));

var_dump(getrandmax());
var_dump(getrandmax(1));

var_dump(mt_getrandmax());
var_dump(mt_getrandmax(1));

echo "Done\n";
?>
--EXPECTF--	
int(%d)

Warning: mt_rand() expects exactly 2 parameters, 1 given in %s on line %d
NULL
int(%i)
int(%d)
int(%d)

Warning: rand() expects exactly 2 parameters, 1 given in %s on line %d
NULL
int(%i)
int(%d)
NULL
NULL

Warning: srand() expects parameter 1 to be integer, array given in %s on line %d
NULL
NULL
NULL

Warning: mt_srand() expects parameter 1 to be integer, array given in %s on line %d
NULL
int(%d)

Warning: getrandmax() expects exactly 0 parameters, 1 given in %s on line %d
NULL
int(%d)

Warning: mt_getrandmax() expects exactly 0 parameters, 1 given in %s on line %d
NULL
Done
