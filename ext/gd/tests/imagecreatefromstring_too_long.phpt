--TEST--
imagecreatefromstring(): a string longer than INT_MAX throws without decoding it
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die('skip 64-bit only');
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--INI--
memory_limit=-1
--FILE--
<?php
$s = str_repeat("x", 2147483648);
try {
    imagecreatefromstring($s);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
echo "Done\n";
?>
--EXPECT--
imagecreatefromstring(): Argument #1 ($data) is too long
Done
