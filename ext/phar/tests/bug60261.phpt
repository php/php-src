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

Warning: Phar::__construct() expects at least 1 parameter, 0 given in %s on line %d
SplFileInfo::getLinkTarget(): Empty filename
