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
try {
    srand(mode: MT_RAND_MT19937);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump(mt_srand());
var_dump(mt_srand(-1));

try {
    mt_srand(mode: MT_RAND_MT19937);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

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
srand(): Argument #1 ($seed) must be passed explicitly, because the default value is not known
NULL
NULL
mt_srand(): Argument #1 ($seed) must be passed explicitly, because the default value is not known
int(%d)
int(%d)
Done
