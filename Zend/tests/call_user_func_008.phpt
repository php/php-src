--TEST--
call_user_func() behavior with references
--FILE--
<?php

function test(&$ref1, &$ref2) {
    $ref1 += 42;
    $ref2 -= 42;
    return true;
}

$i = $j = 0;
var_dump(call_user_func('test', $i, $j));
var_dump($i, $j);

var_dump(call_user_func_array('test', [$i, $j]));
var_dump($i, $j);

$x =& $i; $y =& $j;
var_dump(call_user_func('test', $i, $j));
var_dump($i, $j);

var_dump(call_user_func_array('test', [$i, $j]));
var_dump($i, $j);

?>
--EXPECTF--
Warning: Parameter 1 to test() expected to be a reference, value given in %s on line %d

Warning: Parameter 2 to test() expected to be a reference, value given in %s on line %d
bool(true)
int(0)
int(0)

Warning: Parameter 1 to test() expected to be a reference, value given in %s on line %d

Warning: Parameter 2 to test() expected to be a reference, value given in %s on line %d
bool(true)
int(0)
int(0)

Warning: Parameter 1 to test() expected to be a reference, value given in %s on line %d

Warning: Parameter 2 to test() expected to be a reference, value given in %s on line %d
bool(true)
int(0)
int(0)

Warning: Parameter 1 to test() expected to be a reference, value given in %s on line %d

Warning: Parameter 2 to test() expected to be a reference, value given in %s on line %d
bool(true)
int(0)
int(0)
