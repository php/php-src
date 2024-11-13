--TEST--
Incrementing max int values 32bit
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--INI--
precision=14
--FILE--
<?php

$values = [
    PHP_INT_MAX,
    (string)PHP_INT_MAX
];

foreach ($values as $var) {
    $var++;
    var_dump($var);
}
echo "Done\n";
?>
--EXPECT--
float(2147483648)
float(2147483648)
Done
