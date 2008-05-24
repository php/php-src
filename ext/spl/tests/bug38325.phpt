--TEST--
Bug #38325 (spl_autoload_register() gaves wrong line for "class not found")
--FILE--
<?php
spl_autoload_register();
new Foo();
?>
--EXPECTF--
Fatal error: spl_autoload(): Class Foo could not be loaded in %s on line 3
