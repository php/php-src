--TEST--
testing integer overflow (64bit)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
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

/* ++ and -- overflow to float at the zend_long boundary, and must not do so
 * anywhere below it. The 32-bit boundaries matter on builds where zend_long is
 * wider than the platform word: they are the values a 32-bit increment would
 * either wrap or wrongly report as overflowing. */
$i = PHP_INT_MAX;   $i++; var_dump($i);
$i = PHP_INT_MIN;   $i--; var_dump($i);
$i = 2147483647;    $i++; var_dump($i);
$i = 4294967295;    $i++; var_dump($i);
$i = -2147483648;   $i--; var_dump($i);
$i = -4294967296;   $i--; var_dump($i);

echo "Done\n";
?>
--EXPECTF--
int(9223372036854775807)

Warning: The float %f is not representable as an int, cast occurred in %s on line %d
int(-9223372036854775808)

Warning: The float %f is not representable as an int, cast occurred in %s on line %d
int(-9223372036854775808)

Warning: The float %f is not representable as an int, cast occurred in %s on line %d
int(0)
int(-9223372036854775808)
int(-9223372036854775808)
int(-9223372036854775808)
float(9.223372036854776E+18)
float(-9.223372036854776E+18)
int(2147483648)
int(4294967296)
int(-2147483649)
int(-4294967297)
Done
