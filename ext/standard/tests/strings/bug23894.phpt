--TEST--
Bug #23894 (sprintf() decimal specifiers problem)
--FILE--
<?php
$a = -12.3456;
$test = sprintf("%04d", $a);
var_dump($test, bin2hex($test));
$test = sprintf("% 13u", $a);
var_dump($test, bin2hex($test));
?>
--EXPECT--
string(4) "-012"
string(8) "2d303132"
string(13) "   4294967284"
string(26) "20202034323934393637323834"
