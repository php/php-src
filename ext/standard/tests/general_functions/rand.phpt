--TEST--
rand() and mt_rand() tests
--FILE--
<?php

var_dump(mt_rand());
var_dump(mt_rand(-1,1));
var_dump(mt_rand(0,3));

var_dump(rand());
var_dump(rand(-1,1));
var_dump(rand(0,3));

var_dump(srand());
var_dump(srand(-1));

var_dump(mt_srand());
var_dump(mt_srand(-1));

var_dump(getrandmax());

var_dump(mt_getrandmax());

echo "Done\n";
?>
--EXPECTF--
int(%d)
int(%i)
int(%d)
int(%d)
int(%i)
int(%d)
NULL
NULL
NULL
NULL
int(%d)
int(%d)
Done
