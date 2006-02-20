--TEST--
Bug #32647 (Using register_shutdown_function() with invalid callback can crash PHP)
--INI--
error_reporting=2047
display_errors=1
--FILE--
<?php

function foo()
{
  echo "joo!\n";
}

class bar
{
	function barfoo ()
	{ echo "bar!\n"; }
}

unset($obj);
register_shutdown_function(array($obj,""));            // Invalid
register_shutdown_function(array($obj,"some string")); // Invalid
register_shutdown_function(array(0,""));               // Invalid
register_shutdown_function(array('bar','foo'));        // Invalid
register_shutdown_function(array(0,"some string"));    // Invalid
register_shutdown_function('bar');                     // Valid
register_shutdown_function('foo');                     // Valid
register_shutdown_function(array('bar','barfoo'));     // Valid

$obj = new bar;
register_shutdown_function(array($obj,'foobar'));      // Invalid
register_shutdown_function(array($obj,'barfoo'));      // Valid

?>
--EXPECTF--
Notice: Undefined variable:  obj in %s on line %d

Warning: register_shutdown_function() [/phpmanual/function.register-shutdown-function.html]: Invalid shutdown callback 'Array' passed in %s on line %d

Notice: Undefined variable:  obj in %s on line %d

Warning: register_shutdown_function() [/phpmanual/function.register-shutdown-function.html]: Invalid shutdown callback 'Array' passed in %s on line %d

Warning: register_shutdown_function() [/phpmanual/function.register-shutdown-function.html]: Invalid shutdown callback 'Array' passed in %s on line %d

Warning: register_shutdown_function() [/phpmanual/function.register-shutdown-function.html]: Invalid shutdown callback 'Array' passed in %s on line %d

Warning: (Registered shutdown functions) Unable to call bar::foo() - function does not exist in Unknown on line 0

Warning: (Registered shutdown functions) Unable to call bar() - function does not exist in Unknown on line 0
joo!
bar!

Warning: (Registered shutdown functions) Unable to call bar::foobar() - function does not exist in Unknown on line 0
bar!
