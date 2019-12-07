--TEST--
Calling mb_convert_case() with an invalid casing mode
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php

try {
    var_dump(mb_convert_case("foobar", 100));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Invalid case mode
