--TEST--
Array unpacking does not work with non-integer/string keys
--FILE--
<?php
function gen() {
    yield [] => 1;
    yield 1.23 => 123;
}

try {
    [...gen()];
} catch (Error $ex) {
    echo "Exception: " . $ex->getMessage() . "\n";
}

?>
--EXPECT--
Exception: Keys must be of type int|string during array unpacking
