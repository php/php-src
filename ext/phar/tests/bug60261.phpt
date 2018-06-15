--TEST--
Bug #60261 (phar dos null pointer)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php

try {
    $nx = new Phar();
	$nx->getLinkTarget();
} catch (TypeError $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Phar::__construct() expects at least 1 parameter, 0 given
