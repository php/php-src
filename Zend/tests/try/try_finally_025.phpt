--TEST--
Throw in try of try/finally inside catch
--FILE--
<?php

function test() {
    try {
        throw new Exception(1);
    } catch (Exception $e) {
        try {
            throw new Exception(2);
        } finally {
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
Exception: 2 in %s:%d
Stack trace:
#0 %s(%d): test()
#1 {main}
