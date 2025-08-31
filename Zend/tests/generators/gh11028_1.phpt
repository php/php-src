--TEST--
GH-11028 (Heap Buffer Overflow in zval_undefined_cv with generators) - other types variant
--FILE--
<?php
function generator($x) {
    try {
        yield $x => 0;
    } finally {
        return [];
    }
}

function test($msg, $x) {
    echo "yield $msg\n";
    try {
        var_dump([...generator($x)]);
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }
}

test("null", null);
test("false", false);
test("true", true);
test("object", new stdClass);
?>
--EXPECT--
yield null
Keys must be of type int|string during array unpacking
yield false
Keys must be of type int|string during array unpacking
yield true
Keys must be of type int|string during array unpacking
yield object
Keys must be of type int|string during array unpacking
