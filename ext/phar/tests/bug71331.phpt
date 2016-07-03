--TEST--
Bug #71331 (Uninitialized pointer in phar_make_dirstream())
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
$p = new PharData(__DIR__."/bug71331.tar");
?>
DONE
--EXPECTF--
Fatal error: Uncaught exception 'UnexpectedValueException' with message 'phar error: "%s%ebug71331.tar" is a corrupted tar file (invalid entry size)' in %s%ebug71331.php:2
Stack trace:
#0 %s%ebug71331.php(2): PharData->__construct('%s')
#1 {main}
  thrown in %s%ebug71331.php on line 2
