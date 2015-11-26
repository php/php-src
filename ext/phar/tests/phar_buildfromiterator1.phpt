--TEST--
Phar::buildFromIterator() readonly
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$phar = new Phar(dirname(__FILE__) . '/buildfromiterator1.phar');
try {
	ini_set('phar.readonly', 1);
	$phar->buildFromIterator(1);
} catch (Exception $e) {
	var_dump(get_class($e));
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/buildfromiterator1.phar');
__HALT_COMPILER();
?>
--EXPECTF--
%s(24) "UnexpectedValueException"
Cannot write out phar archive, phar is read-only
===DONE===
