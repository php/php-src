--TEST--
Bug #77396 Relative filename exceeding maximum path length causes null pointer dereference.
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
$path = '../' . str_repeat("x", PHP_MAXPATHLEN) . '.tar';
$phar = new PharData($path);
?>
--EXPECTF--
Fatal error: Uncaught UnexpectedValueException: Phar creation or opening failed in %s%ebug77396.php:%d
Stack trace:
#0 %s%ebug77396.php(%d): PharData->__construct(%s)
#1 {main}
  thrown in %s%ebug77396.php on line %d
