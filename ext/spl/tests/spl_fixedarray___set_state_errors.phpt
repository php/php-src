--TEST--
SplFixedArray: __set_state
--FILE--
<?php

try {
    $fixed = SplFixedArray::__set_state([1 => 2]);
} catch (ValueError $error) {
    var_dump($error->getMessage());
}

try {
    $fixed = SplFixedArray::__set_state([0 => 1, 1 => 2, 3 => 4]);
} catch (ValueError $error) {
    var_dump($error->getMessage());
}

try {
    $fixed = SplFixedArray::__set_state([-1 => 2]);
} catch (ValueError $error) {
    var_dump($error->getMessage());
}

echo "-----\n";

try {
    $fixed = SplFixedArray::__set_state([0 => 1, 'type' => 'undefined']);
} catch (ValueError $error) {
    var_dump($error->getMessage());
}
--EXPECTF--
string(%d) "SplFixedArray::__set_state(): Argument #1 ($state) did not have integer keys that start at 0 and increment sequentially"
string(%d) "SplFixedArray::__set_state(): Argument #1 ($state) did not have integer keys that start at 0 and increment sequentially"
string(%d) "SplFixedArray::__set_state(): Argument #1 ($state) did not have integer keys that start at 0 and increment sequentially"
-----
string(%d) "SplFixedArray::__set_state(): Argument #1 ($state) must have all its string keys come before all integer keys"
