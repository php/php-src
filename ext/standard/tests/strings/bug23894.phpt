--TEST--
Bug #23894 (sprintf() decimal specifiers problem)
--FILE--
<?php
$a = -12.3456;
$test = sprintf("%04d", $a);
var_dump($test, bin2hex((binary)$test));
$test = sprintf("% 13u", $a);
var_dump($test, bin2hex((binary)$test));
?>
--EXPECTREGEX--
unicode\(4\) \"-012\"
unicode\(8\) \"2d303132\"
(unicode\(13\) \"   4294967284\"|unicode\(20\) \"18446744073709551604\")
(unicode\(26\) \"20202034323934393637323834\"|unicode\(40\) \"3138343436373434303733373039353531363034\")
