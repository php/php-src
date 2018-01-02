--TEST--
gmp_setbit() with large index
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
<?php if (getenv("SKIP_SLOW_TESTS")) die("skip slow test"); ?>
--FILE--
<?php

$n = gmp_init("227200");
for($a = 1<<30; $a > 0 && $a < 0x8000000000; $a <<= 2) {
	$i = $a - 1;
    printf("%X\n", $i);
    gmp_setbit($n, $i, 1);
}
echo "Done\n";
?>
--EXPECTF--
3FFFFFFF
FFFFFFFF
3FFFFFFFF
FFFFFFFFF
3FFFFFFFFF

Warning: gmp_setbit(): Index must be less than %d * %d in %s/gmp_setbit_long.php on line %d
Done