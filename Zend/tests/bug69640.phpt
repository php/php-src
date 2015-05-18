--TEST--
Bug #69640 Unhandled EngineExceptions thrown from userland do not produce any output
--FILE--
<?php
throw new \ParseException('I mess everything up! :trollface:');
?>
--EXPECTF--
Fatal error: I mess everything up! :trollface: in %sbug69640.php on line 2
