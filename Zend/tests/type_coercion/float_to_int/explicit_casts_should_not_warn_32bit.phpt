--TEST--
Explicit (int) cast must not warn 32bit variation
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php

$values =[
    3.0,
    3.5,
    10e120,
    10e300,
    fdiv(0, 0),
    (string) 3.0,
    (string) 3.5,
    (string) 10e120,
    (string) 10e300,
    (string) fdiv(0, 0),
];

foreach($values as $value) {
    var_dump((int) $value);
}

?>
--EXPECT--
int(3)
int(3)
int(0)
int(0)
int(0)
int(3)
int(3)
int(2147483647)
int(2147483647)
int(0)
