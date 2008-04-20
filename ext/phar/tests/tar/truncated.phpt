--TEST--
Phar: truncated tar
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
	$p = new PharData(dirname(__FILE__) . '/files/trunc.tar');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar');
?>
--EXPECTF--
phar error: "%strunc.tar" is a corrupted tar file (truncated)
===DONE===
