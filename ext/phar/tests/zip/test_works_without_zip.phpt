--TEST--
Phar with zip disabled
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (extension_loaded("zip")) die("skip"); ?>
--FILE--
<?php
try {
	$phar = new Phar(dirname(__FILE__) . '/frontcontroller1.phar.zip');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
Cannot open phar file '%sfrontcontroller1.phar.zip' with alias '(null)': phar zip error: phar "%sfrontcontroller1.phar.zip" cannot be created as zip-based phar, zip-based phars are disabled (enable ext/zip)
===DONE===
