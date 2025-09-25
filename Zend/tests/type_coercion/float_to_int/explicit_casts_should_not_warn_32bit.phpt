--TEST--
Explicit (int) cast must not warn if value is representable 32bit variation
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
--EXPECTF--
Warning: unexpected NAN value was coerced to string in %s on line %d
int(3)
int(3)

Warning: The float 1.0E+121 is not representable as an int, cast occurred in %s on line %d
int(0)

Warning: The float 1.0E+301 is not representable as an int, cast occurred in %s on line %d
int(0)

Warning: The float NAN is not representable as an int, cast occurred in %s on line %d
int(0)
int(3)
int(3)

Warning: The float-string "1.0E+121" is not representable as an int, cast occurred in %s on line %d
int(2147483647)

Warning: The float-string "1.0E+301" is not representable as an int, cast occurred in %s on line %d
int(2147483647)
int(0)
