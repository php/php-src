--TEST--
Hexadecimal integer strings (64bit)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Using hexadecimal prefix notation lowercase */
/* Maximum value representable as integer */
$hex = 0x7FFFFFFFFFFFFFFF;
var_dump($hex);
var_dump(PHP_INT_MAX);

/* Floating number */
$hex = 0x45FFFABCDE0000000;
var_dump($hex);

/* Integer */
$hex = 0x1C;
var_dump($hex);

/* underscore separator */
$hex = 0x1_C;
var_dump($hex);

/* Ignore leading 0 and _ */
$hex = 0x0_01C;
var_dump($hex);
$hex = 0x0_1C;
var_dump($hex);

/* Overflow to infinity */
$hex = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF;
var_dump($hex);


/* Using hexadecimal prefix notation uppercase */
/* Maximum value representable as integer */
$hex = 0X7FFFFFFFFFFFFFFF;
var_dump($hex);
var_dump(PHP_INT_MAX);

/* Floating number */
$hex = 0X45FFFABCDE0000000;
var_dump($hex);

/* Integer */
$hex = 0X1C;
var_dump($hex);

/* underscore separator */
$hex = 0X1_C;
var_dump($hex);

/* Ignore leading 0 and _ */
$hex = 0X0_01C;
var_dump($hex);
$hex = 0X0_1C;
var_dump($hex);

/* Overflow to infinity */
$hex = 0XFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF;
var_dump($hex);

?>
--EXPECT--
int(9223372036854775807)
int(9223372036854775807)
float(8.070441274821732E+19)
int(28)
int(28)
int(28)
int(28)
float(INF)
int(9223372036854775807)
int(9223372036854775807)
float(8.070441274821732E+19)
int(28)
int(28)
int(28)
int(28)
float(INF)
