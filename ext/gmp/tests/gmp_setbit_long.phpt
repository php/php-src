--TEST--
gmp_setbit() with large index
--EXTENSIONS--
gmp
--SKIPIF--
<?php
if (getenv("TRAVIS") === "true") die("skip not suitable for Travis-CI");
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
/* This test requires about 8G RAM which likely not to be present on an arbitrary CI. */
include(__DIR__.'/../../../tests/utils.php');
if(!has_enough_memory(8 * 1024 * 1024 * 1024)) {
    die('skip Reason: Insufficient RAM (should be 8GB)');
}
?>
--FILE--
<?php

$n = gmp_init("227200");
for($a = 1<<30; $a > 0 && $a < 0x8000000000; $a <<= 2) {
    $i = $a - 1;
    printf("%X\n", $i);
    try {
        gmp_setbit($n, $i, 1);
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}
echo "Done\n";
?>
--EXPECTF--
3FFFFFFF
FFFFFFFF
3FFFFFFFF
FFFFFFFFF
3FFFFFFFFF
gmp_setbit(): Argument #2 ($index) must be less than %d * %d
Done
