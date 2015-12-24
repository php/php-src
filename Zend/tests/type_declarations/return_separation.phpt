--TEST--
Return values are separated for references with rc=1
--FILE--
<?php

function test1() : array {
    $array = [];
    $ref =& $array;
    unset($ref);
    return $array;
}

function test2() : string {
    $int = 42;
    $ref =& $int;
    unset($ref);
    return $int;
}

var_dump(test1());
var_dump(test2());

?>
--EXPECT--
array(0) {
}
string(2) "42"
