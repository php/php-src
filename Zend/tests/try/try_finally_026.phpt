--TEST--
Throw in finally inside catch inside finally
--FILE--
<?php

function test() {
    try {
        throw new Exception(1);
    } finally {
        try {
            throw new Exception(2);
        } catch (Exception $e) {
            try {
            } finally {
                throw new Exception(3);
            }
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
Exception: 1 in %s:%d
Stack trace:
#0 %s(%d): test()
#1 {main}

Next Exception: 3 in %s:%d
Stack trace:
#0 %s(%d): test()
#1 {main}
