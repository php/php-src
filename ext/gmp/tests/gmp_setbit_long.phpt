--TEST--
gmp_setbit() with large index
--EXTENSIONS--
gmp
--SKIPIF--
<?php if (getenv("TRAVIS") === "true") die("skip not suitable for Travis-CI"); ?>
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
<?php if (getenv("SKIP_SLOW_TESTS")) die("skip slow test"); ?>
<?php
    /* This test requires about 8G RAM which likely not to be present on an arbitrary CI. */
    if (!file_exists("/proc/meminfo")) {
        die("skip cannot determine free memory amount.");
    }
    $s = file_get_contents("/proc/meminfo");
    $free = 0;
    if (preg_match(",MemFree:\s+(\d+)\s+kB,", $s, $m)) {
        /* Got amount in kb. */
        $free = $m[1]/1024/1024;
    }
    if ($free < 8) {
        die("skip not enough free RAM.");
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
