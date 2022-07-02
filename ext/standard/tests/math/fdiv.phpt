--TEST--
fdiv() function
--FILE--
<?php

var_dump(fdiv(10, 3));
var_dump(fdiv(10., 3.));
var_dump(fdiv(-10., 2.5));
var_dump(fdiv(10., -2.5));
echo "\n";
var_dump(fdiv(10., 0.));
var_dump(fdiv(10., -0.));
var_dump(fdiv(-10., 0.));
var_dump(fdiv(-10., -0.));
echo "\n";
var_dump(fdiv(INF, 0.));
var_dump(fdiv(INF, -0.));
var_dump(fdiv(-INF, 0.));
var_dump(fdiv(-INF, -0.));
echo "\n";
var_dump(fdiv(0., 0.));
var_dump(fdiv(0., -0.));
var_dump(fdiv(-0., 0.));
var_dump(fdiv(-0., -0.));
echo "\n";
var_dump(fdiv(INF, INF));
var_dump(fdiv(INF, -INF));
var_dump(fdiv(-INF, INF));
var_dump(fdiv(-INF, -INF));
echo "\n";
var_dump(fdiv(0., INF));
var_dump(fdiv(0., -INF));
var_dump(fdiv(-0., INF));
var_dump(fdiv(-0., -INF));
echo "\n";
var_dump(fdiv(NAN, NAN));
var_dump(fdiv(INF, NAN));
var_dump(fdiv(0., NAN));
var_dump(fdiv(NAN, INF));
var_dump(fdiv(NAN, 0.));

?>
--EXPECT--
float(3.3333333333333335)
float(3.3333333333333335)
float(-4)
float(-4)

float(INF)
float(-INF)
float(-INF)
float(INF)

float(INF)
float(-INF)
float(-INF)
float(INF)

float(NAN)
float(NAN)
float(NAN)
float(NAN)

float(NAN)
float(NAN)
float(NAN)
float(NAN)

float(0)
float(-0)
float(-0)
float(0)

float(NAN)
float(NAN)
float(NAN)
float(NAN)
float(NAN)
