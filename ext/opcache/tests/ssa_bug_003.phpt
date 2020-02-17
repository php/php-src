--TEST--
Incorrect elision of return type checks
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function test1($x) : callable {
    if ($x == 1) {
        $c = 'foo';
    } elseif ($x == 2) {
        $c = new stdClass;
    } else {
        $c = [$x => &$x];
    }
    return $c;
}

try {
    test1(1);
} catch (Error $e) {
    echo "Error: {$e->getMessage()}\n";
}

class Foo {}
function test2() : Foo {
    $obj = new stdClass;
    return $obj;
}

try {
    test2();
} catch (Error $e) {
    echo "Error: {$e->getMessage()}\n";
}

?>
--EXPECT--
Error: Return value of test1() must be of type callable, string returned
Error: Return value of test2() must be of type Foo, stdClass returned
