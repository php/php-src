--TEST--
Bug #38325 (spl_autoload_register() gaves wrong line for "class not found")
--FILE--
<?php
spl_autoload_register();
new Foo();
?>
--EXPECTF--
Fatal error: Class 'Foo' not found in %s on line %d
