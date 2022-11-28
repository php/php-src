--TEST--
Bug #71204 (segfault if clean autoloaders while autoloading)
--FILE--
<?php

autoload_register_class(function ($name) {
    autoload_unregister_class("autoload_unregister_class");
});

autoload_register_class(function ($name) {
});

new A();
?>
--EXPECTF--
Fatal error: Uncaught Error: Class "A" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %sbug71204.php on line %d
