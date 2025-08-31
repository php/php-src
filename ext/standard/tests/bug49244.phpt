--TEST--
Bug #49244 (Floating point NaN cause garbage characters)
--FILE--
<?php

for ($i = 0; $i < 10; $i++) {
    printf("{%f} %1\$f\n", pow(-1.0, 0.3));
    printf("{%f} %1\$f\n", pow(-1.0, 0.3));
}

?>
--EXPECT--
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
{NaN} NaN
