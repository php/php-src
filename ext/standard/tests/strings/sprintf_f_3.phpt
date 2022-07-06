--TEST--
sprintf %f #3
--SKIPIF--
<?php if(false == setlocale(LC_NUMERIC, "is_IS", "is_IS.UTF-8")) print "skip icelandic locale not supported"; ?>
--FILE--
<?php
setlocale(LC_NUMERIC, "is_IS", "is_IS.UTF-8");
var_dump(sprintf("%.3f", 100.426));
var_dump(sprintf("%.2f", 100.426));
var_dump(sprintf("%f'",  100.426));

$money1 = 68.75;
$money2 = 54.35;
$money = $money1 + $money2;
var_dump(sprintf("%01.2f", $money));
var_dump(sprintf("%.3e", $money));
?>
--EXPECT--
string(7) "100,426"
string(6) "100,43"
string(11) "100,426000'"
string(6) "123,10"
string(8) "1.231e+2"
