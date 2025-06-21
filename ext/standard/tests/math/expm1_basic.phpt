--TEST--
Test expm1() - basic function test for expm1()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach($values as $value) {
    var_dump(expm1($value));
};
?>
--EXPECT--
float(9744803445.248903)
float(-0.9999999998973812)
float(15282893988.958735)
float(15282893988.958735)
float(-0.9999999999345673)
float(22025.465794806718)
float(29731.618852891435)
float(INF)
