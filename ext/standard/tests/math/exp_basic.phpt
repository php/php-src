--TEST--
Test exp() - basic function test for exp()
--INI--
precision=14
--FILE--
<?php
$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = exp($value);
    var_dump($res);
}

?>
--EXPECT--
float(9744803446.248903)
float(1.026187963170189E-10)
float(15282893989.958735)
float(15282893989.958735)
float(6.543263341727205E-11)
float(22026.465794806718)
float(29732.618852891435)
float(INF)
