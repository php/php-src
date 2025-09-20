--TEST--
testing integer overflow (32bit)
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--FILE--
<?php

$doubles = array(
    2147483648,
    2147483649,
    2147483658,
    2147483748,
    2147484648,
    );

foreach ($doubles as $d) {
    $l = (int)$d;
    var_dump($l);
}

echo "Done\n";
?>
--EXPECTF--
Warning: non-representable float 2147483648 was cast to int in %s on line %d
int(-2147483648)

Warning: non-representable float 2147483649 was cast to int in %s on line %d
int(-2147483647)

Warning: non-representable float 2147483658 was cast to int in %s on line %d
int(-2147483638)

Warning: non-representable float 2147483748 was cast to int in %s on line %d
int(-2147483548)

Warning: non-representable float 2147484648 was cast to int in %s on line %d
int(-2147482648)
Done
