--TEST--
Phar::buildFromDirectory() - non-directory passed as first parameter
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
try {
	$phar = new Phar(dirname(__FILE__) . '/buildfromdirectory.phar');
	$phar->buildFromDirectory(1);
} catch (Exception $e) {
	var_dump(get_class($e));
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/buildfromdirectory.phar');
__HALT_COMPILER();
?>
--EXPECTF--
%s(24) "UnexpectedValueException"
RecursiveDirectoryIterator::__construct(1): failed to open dir: No such file or directory
===DONE===
