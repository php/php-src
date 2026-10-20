--TEST--
Return in try with throw in finally, inside other finally
--FILE--
<?php

function test() {
    try {
        throw new Exception(1);
    } finally {
        try {
            return 42;
        } finally {
            throw new Exception(2);
        }
    }
}

try {
    test();
} catch (Exception $e) {
    echo $e, "\n";
}

?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d
Exception: 1 in %s:%d
Stack trace:
#0 %s(%d): test()
#1 {main}

Next Exception: 2 in %s:%d
Stack trace:
#0 %s(%d): test()
#1 {main}
