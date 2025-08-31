--TEST--
array_pad() with too large padding should fail
--FILE--
<?php

function test($length) {
    try {
        var_dump(array_pad(array("", -1, 2.0), $length, 0));
    } catch (\ValueError $e) {
        echo $e->getMessage() . "\n";
    }
}

test(PHP_INT_MIN);
test(PHP_INT_MAX);

?>
--EXPECT--
array_pad(): Argument #2 ($length) must not exceed the maximum allowed array size
array_pad(): Argument #2 ($length) must not exceed the maximum allowed array size
