--TEST--
Phar::buildFromDirectory() - object passed as second parameter
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

try {
	$phar = new Phar(dirname(__FILE__) . '/buildfromdirectory3.phar');
	$phar->buildFromDirectory('files', new stdClass);
} catch (Exception $e) {
	var_dump(get_class($e));
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/buildfromdirectory3.phar');
__HALT_COMPILER();
?>
--EXPECTF--
Warning: Phar::buildFromDirectory() expects parameter 2 to be %string, object given in %sphar_buildfromdirectory3.php on line %d
===DONE===
