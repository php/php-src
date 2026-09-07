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
Deprecated: spl_autoload_unregister(): Using spl_autoload_call() as a callback for spl_autoload_unregister() is deprecated, to remove all registered autoloaders, call spl_autoload_unregister() for all values returned from spl_autoload_functions() in %s on line %d

Fatal error: Uncaught Error: Class "A" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %sbug71204.php on line %d
