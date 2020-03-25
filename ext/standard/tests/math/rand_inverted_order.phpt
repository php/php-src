--TEST--
Test rand function when min and max are in proper or inverted order
--CREDITS--
PHP TestFEst 2017 - PHPDublin, PHPSP - Joao P V Martins <jp.joao@gmail.com>
--FILE--
<?php
$min_value = 10;
$max_value = 20;
$correct_order = rand($min_value, $max_value);
$incorrect_order = rand($max_value, $min_value);
var_dump($correct_order >= $min_value);
var_dump($correct_order <= $max_value);
var_dump($incorrect_order >= $min_value);
var_dump($incorrect_order <= $max_value);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
