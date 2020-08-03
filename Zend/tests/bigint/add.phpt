--TEST--
Bigint addition tests
--SKIPIF--
<?php if (!extension_loaded('gmp')) { die('skip requires gmp'); } ?>
--FILE--
<?php
echo 1_2_3n+1n, "\n";
var_dump(0700n + 0004n);
echo 2n ** 100, "\n";
function sum_cube(GMP $a, GMP $b): GMP {
    return $a ** 3 + $b ** 3;
}
echo sum_cube(3n, 9000000000000n), "\n";

?>
--EXPECTF--
124
object(GMP)#1 (1) {
  ["num"]=>
  string(3) "452"
}
1267650600228229401496703205376
729000000000000000000000000000000000027
