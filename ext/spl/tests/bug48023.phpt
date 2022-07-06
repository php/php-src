--TEST--
Bug #48023 (spl_autoload_register didn't addref closures)
--FILE--
<?php
spl_autoload_register(function(){});

new Foo;

?>
===DONE===
--EXPECTF--
Fatal error: Uncaught Error: Class "Foo" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
