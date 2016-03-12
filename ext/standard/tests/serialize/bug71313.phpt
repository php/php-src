--TEST--
Bug #71311 Use-after-free vulnerability in SPL(SplObjectStorage, unserialize)
--FILE--
<?php
$data = unserialize("C:16:\"SplObjectStorage\":113:{x:i:2;O:8:\"stdClass\":0:{},a:2:{s:4:\"prev\";i:2;s:4:\"next\";O:8:\"stdClass\":0:{}};r:7;,R:2;s:4:\"next\";;r:3;};m:a:0:{}}");
var_dump($data);
?>
--EXPECTF--
Fatal error: Uncaught UnexpectedValueException: Error at offset 82 of 113 bytes in %s%ebug71313.php:2
Stack trace:
#0 [internal function]: SplObjectStorage->unserialize('%s')
#1 %s%ebug71313.php(2): unserialize('%s')
#2 {main}
  thrown in %s%ebug71313.php on line 2
