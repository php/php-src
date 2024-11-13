--TEST--
Decrementing min int values 32bit
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
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
float(-2147483649)
float(-2147483649)
Done
