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
} catch (Throwable $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}

?>
--EXPECT--
Error: Keys must be of type int|string during array unpacking
