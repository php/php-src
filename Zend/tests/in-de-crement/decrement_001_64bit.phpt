--TEST--
Decrementing min int values 64bit
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--INI--
precision=14
--FILE--
<?php

$values = [
    -PHP_INT_MAX-1,
    (string)(-PHP_INT_MAX-1),
];

foreach ($values as $var) {
    $var--;
    var_dump($var);
}

echo "Done\n";
?>
--EXPECT--
float(-9.223372036854776E+18)
float(-9.223372036854776E+18)
Done
