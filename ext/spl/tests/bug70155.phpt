--TEST--
SPL: Bug #70155 Use After Free Vulnerability in unserialize() with SPLArrayObject
--FILE--
<?php
$inner = 'x:i:0;O:12:"DateInterval":1:{s:1:"y";i:3;};m:a:1:{i:0;R:2;}';
$exploit = 'C:11:"ArrayObject":'.strlen($inner).':{'.$inner.'}';
$data = unserialize($exploit);

var_dump($data);
?>
--EXPECTF--
Fatal error: Uncaught InvalidArgumentException: Overloaded object of type DateInterval is not compatible with ArrayObject in %s
Stack trace:
%s
%s
%s
%s
