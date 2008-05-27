--TEST--
sprintf %f #3
--INI--
error_reporting=6143
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
unicode(7) "100,426"
unicode(6) "100,43"
unicode(11) "100,426000'"
unicode(6) "123,10"
unicode(8) "1.231e+2"
