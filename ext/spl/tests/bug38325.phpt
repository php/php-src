--TEST--
Bug #38325 (spl_autoload_register() gaves wrong line for "class not found")
--FILE--
<?php
spl_autoload_register();
new ThisClassDoesNotExistEverFoo();
?>
--EXPECTF--
Fatal error: Uncaught Error: Class ThisClassDoesNotExistEverFoo could not be loaded in %s:%d
Stack trace:
#0 %s(%d): spl_autoload('ThisClassDoesNo...')
#1 {main}
  thrown in %s on line %d
