--TEST--
Bug #69640 Unhandled Error thrown from userland do not produce any output
--FILE--
<?php
throw new \ParseError('I mess everything up! :trollface:');
?>
--EXPECTF--
Parse error: I mess everything up! :trollface: in %sbug69640.php on line 2
