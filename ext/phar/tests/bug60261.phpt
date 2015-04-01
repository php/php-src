--TEST--
Bug #60261 (phar dos null pointer)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php

$nx = new Phar();
try {
	$nx->getLinkTarget();
} catch (Exception $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Fatal error: Uncaught exception 'PharException' with message 'Phar::__construct() expects at least 1 parameter, 0 given' in %sbug60261.php:3
Stack trace:
#0 %sbug60261.php(3): Phar->__construct()
#1 {main}
  thrown in %sbug60261.php on line 3
