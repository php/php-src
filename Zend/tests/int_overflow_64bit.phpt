--TEST--
testing integer overflow
--FILE--
<?php

$doubles = array(
        PHP_INT_MAX,
        PHP_INT_MAX + 1,
        PHP_INT_MAX + 1000,
        PHP_INT_MAX * 2 + 4,
        -PHP_INT_MAX -1,
        -PHP_INT_MAX -2,
        -PHP_INT_MAX -1000,
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
Done
