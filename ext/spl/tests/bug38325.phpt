--TEST--
Bug #38325 (spl_autoload_register() gaves wrong line for "class not found")
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
spl_autoload_register();
new ThisClassDoesNotExistEverFoo();
?>
--EXPECTF--
Fatal error: spl_autoload(): Class ThisClassDoesNotExistEverFoo could not be loaded in %s on line 3
