--TEST--
Phar::buildFromIterator() wrong object
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
try {
	$phar = new Phar(dirname(__FILE__) . '/buildfromiterator.phar');
	$phar->buildFromIterator(new stdClass);
} catch (Exception $e) {
	var_dump(get_class($e));
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/buildfromiterator.phar');
__HALT_COMPILER();
?>
--EXPECTF--
Warning: Phar::buildFromIterator() expects parameter 1 to be Traversable, object given in %sphar_buildfromiterator2.php on line %d
===DONE===
