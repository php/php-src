--TEST--
Bug #71311 Use-after-free vulnerability in SPL(ArrayObject, unserialize)
--FILE--
<?php
$data = unserialize("C:11:\"ArrayObject\":11:{x:i:0;r:3;XX");
var_dump($data);
?>
--EXPECTF--
Fatal error: Uncaught UnexpectedValueException: Error at offset 10 of 11 bytes in %s%ebug71311.php:2
Stack trace:
#0 [internal function]: ArrayObject->unserialize('x:i:0;r:3;X')
#1 %s%ebug71311.php(2): unserialize('%s')
#2 {main}
  thrown in %s%ebug71311.php on line 2


