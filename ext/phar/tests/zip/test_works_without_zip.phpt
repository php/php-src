--TEST--
Phar with zip disabled
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (extension_loaded("zip")) die("skip"); ?>
--FILE--
<?php
try {
	$phar = new Phar(dirname(__FILE__) . '/frontcontroller.phar.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
Cannot open phar file '%sfrontcontroller.phar.zip' with alias '(null)': phar zip error: cannot open zip-based phar "%sfrontcontroller.phar.zip", ext/zip is not enabled
===DONE===
