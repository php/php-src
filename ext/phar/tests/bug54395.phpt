--TEST--
Bug #54395 (Phar::mount() crashes when calling with wrong parameters)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php

try {
	phar::mount(1,1);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

?>
--EXPECTF--
string(25) "Mounting of 1 to 1 failed"
