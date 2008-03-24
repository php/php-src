--TEST--
Phar: set alias with invalid alias containing / \ : or ;
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar';

$p = new Phar($fname);
try {
	$p->setAlias('hi/');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	$p->setAlias('hi\\l');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

try {
	$p->setAlias('hil;');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

try {
	$p->setAlias(':hil');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar');
?>
--EXPECTF--
Invalid alias "hi/" specified for phar "%sinvalid_alias.phar"
Invalid alias "hi\l" specified for phar "%sinvalid_alias.phar"
Invalid alias "hil;" specified for phar "%sinvalid_alias.phar"
Invalid alias ":hil" specified for phar "%sinvalid_alias.phar"
===DONE===