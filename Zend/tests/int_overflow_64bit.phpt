--TEST--
testing integer overflow
--FILE--
<?php

const PHP_INT_MAX64 = 0x7FFFFFFFFFFFFFFF;
const PHP_INT_MAX32 = 0x7FFFFFFF;

$doubles = array(
        PHP_INT_MAX64,
        PHP_INT_MAX64 + 1,
        PHP_INT_MAX64 + 1000,
        PHP_INT_MAX64 * 2 + 4,
        -PHP_INT_MAX64 -1,
        -PHP_INT_MAX64 -2,
        -PHP_INT_MAX64 -1000,
        PHP_INT_MAX32,
        PHP_INT_MAX32 + 1,
        PHP_INT_MAX32 + 1000,
        PHP_INT_MAX32 * 2 + 4,
        -PHP_INT_MAX32 -1,
        -PHP_INT_MAX32 -2,
        -PHP_INT_MAX32 -1000,
        );

foreach ($doubles as $d) {
        $l = (int)$d;
        var_dump($l);
}

echo "Done\n";
?>
--EXPECT--
int(9223372036854775807)
int(9223372036854775808)
int(9223372036854776807)
int(18446744073709551618)
int(-9223372036854775808)
int(-9223372036854775809)
int(-9223372036854776807)
int(2147483647)
int(2147483648)
int(2147484647)
int(4294967298)
int(-2147483648)
int(-2147483649)
int(-2147484647)
Done
