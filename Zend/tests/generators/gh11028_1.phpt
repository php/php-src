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
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

test("null", null);
test("false", false);
test("true", true);
test("object", new stdClass);
?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d
yield null
Error: Keys must be of type int|string during array unpacking
yield false
Error: Keys must be of type int|string during array unpacking
yield true
Error: Keys must be of type int|string during array unpacking
yield object
Error: Keys must be of type int|string during array unpacking
