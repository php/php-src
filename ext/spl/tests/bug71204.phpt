--TEST--
Bug #71204 (segfault if clean spl_autoload_funcs while autoloading )
--FILE--
<?php

spl_autoload_register(function ($name) {
	spl_autoload_unregister("spl_autoload_call");
});

spl_autoload_register(function ($name) {
});

new A();
?>
--EXPECTF--
Fatal error: Cannot destroy active lambda function in %sbug71204.php on line %d
