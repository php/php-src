--TEST--
Phar: disallow stub and alias setting via offset*() methods
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=1
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://'.$fname;

$phar = new Phar($fname);
$phar->setDefaultStub();
$phar->setAlias('susan');
$phar['a.txt'] = "first file\n";
$phar['b.txt'] = "second file\n";

try {
	$phar->offsetGet('.phar/stub.php');
} catch (Exception $e) {
	echo $e->getMessage()."\n";
}

try {
	$phar->offsetGet('.phar/alias.txt');
} catch (Exception $e) {
	echo $e->getMessage()."\n";
}

try {
	$phar->offsetSet('.phar/stub.php', '<?php __HALT_COMPILER(); ?>');
} catch (Exception $e) {
	echo $e->getMessage()."\n";
}

var_dump(strlen($phar->getStub()));

try {
	$phar->offsetUnset('.phar/stub.php');
} catch (Exception $e) {
	echo $e->getMessage()."\n";
}

var_dump(strlen($phar->getStub()));

try {
	$phar->offsetSet('.phar/alias.txt', 'dolly');
} catch (Exception $e) {
	echo $e->getMessage()."\n";
}

var_dump($phar->getAlias());

try {
	$phar->offsetUnset('.phar/alias.txt');
} catch (Exception $e) {
	echo $e->getMessage()."\n";
}

var_dump($phar->getAlias());

?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
Entry .phar/stub.php does not exist
Entry .phar/alias.txt does not exist
Cannot set stub ".phar/stub.php" directly in phar "%sphar_offset_check.phar.php", use setStub
int(6643)
int(6643)
Cannot set alias ".phar/alias.txt" directly in phar "%sphar_offset_check.phar.php", use setAlias
string(5) "susan"
string(5) "susan"
===DONE===
