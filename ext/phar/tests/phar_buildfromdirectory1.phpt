--TEST--
Phar::buildFromDirectory() - readonly
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$phar = new Phar(dirname(__FILE__) . '/buildfromdirectory1.phar');
try {
	ini_set('phar.readonly', 1);
	$phar->buildFromDirectory(1);
} catch (Exception $e) {
	var_dump(get_class($e));
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/buildfromdirectory1.phar');
__HALT_COMPILER();
?>
--EXPECTF--
%s(24) "UnexpectedValueException"
Cannot write to archive - write operations restricted by INI setting
===DONE===
