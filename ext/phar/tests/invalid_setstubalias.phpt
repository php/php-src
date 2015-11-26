--TEST--
Phar: invalid set alias or stub via array access
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.tar';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip';

$p = new Phar($fname);
try {
	$p['.phar/stub.php'] = 'hi';
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	$p['.phar/alias.txt'] = 'hi';
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
$p = new Phar($fname2);
try {
	$p['.phar/stub.php'] = 'hi';
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	$p['.phar/alias.txt'] = 'hi';
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip');
?>
--EXPECTF--
Cannot set stub ".phar/stub.php" directly in phar "%sinvalid_setstubalias.phar.tar", use setStub
Cannot set alias ".phar/alias.txt" directly in phar "%sinvalid_setstubalias.phar.tar", use setAlias
Cannot set stub ".phar/stub.php" directly in phar "%sinvalid_setstubalias.phar.zip", use setStub
Cannot set alias ".phar/alias.txt" directly in phar "%sinvalid_setstubalias.phar.zip", use setAlias
===DONE===