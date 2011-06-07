--TEST--
Bug #55007 (compiler fail after previous fail)
--FILE--
<?php

function __autoload($classname) {
  if ('CompileErrorClass'==$classname) eval('class CompileErrorClass { function foo() { $a[] } }');
  if ('MyErrorHandler'==$classname) eval('class MyErrorHandler { function __construct() { print "My error handler runs.\n"; } }');
}

function shutdown() {
  new MyErrorHandler();
}


register_shutdown_function('shutdown');
	
new CompileErrorClass();

?>
--EXPECTF--
Fatal error: Cannot use [] for reading in %s(%d) : eval()'d code on line %d
My error handler runs.
