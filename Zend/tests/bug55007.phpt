--TEST--
Bug #55007 (compiler fail after previous fail)
--FILE--
<?php

spl_autoload_register(function ($classname) {
  if ('CompileErrorClass'==$classname) eval('class int {}');
  if ('MyErrorHandler'==$classname) eval('class MyErrorHandler { function __construct() { print "My error handler runs.\n"; } }');
});

function shutdown() {
  new MyErrorHandler();
}


register_shutdown_function('shutdown');

new CompileErrorClass();

?>
--EXPECTF--
Fatal error: Cannot use 'int' as class name as it is reserved in %s on line %d
My error handler runs.
