--TEST--
Bug #72787 (json_decode reads out of bounds)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php

try {
    var_dump(json_decode('[]', false, 0x100000000));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
json_decode(): Argument #3 ($depth) must be less than %d
