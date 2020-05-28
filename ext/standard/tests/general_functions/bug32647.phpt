--TEST--
Bug #32647 (Using register_shutdown_function() with invalid callback can crash PHP)
--FILE--
<?php

function foo()
{
  echo "foo!\n";
}

class bar
{
    function barfoo ()
    { echo "bar!\n"; }
}

unset($obj);
try {
    register_shutdown_function(array($obj,""));            // Invalid
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    register_shutdown_function(array($obj,"some string")); // Invalid
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    register_shutdown_function(array(0,""));               // Invalid
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    register_shutdown_function(array('bar','foo'));        // Invalid
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    register_shutdown_function(array(0,"some string"));    // Invalid
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    register_shutdown_function('bar');                     // Invalid
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

register_shutdown_function('foo');                     // Valid

try {
    register_shutdown_function(array('bar','barfoo'));     // Invalid
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

$obj = new bar;

try {
    register_shutdown_function(array($obj,'foobar'));      // Invalid
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

register_shutdown_function(array($obj,'barfoo'));      // Valid

?>
--EXPECTF--
Warning: Undefined variable $obj in %s on line %d
register_shutdown_function(): Argument #1 ($function) must be of type callable, array given

Warning: Undefined variable $obj in %s on line %d
register_shutdown_function(): Argument #1 ($function) must be of type callable, array given
register_shutdown_function(): Argument #1 ($function) must be of type callable, array given
register_shutdown_function(): Argument #1 ($function) must be of type callable, array given
register_shutdown_function(): Argument #1 ($function) must be of type callable, array given
register_shutdown_function(): Argument #1 ($function) must be of type callable, string given
register_shutdown_function(): Argument #1 ($function) must be of type callable, array given
register_shutdown_function(): Argument #1 ($function) must be of type callable, array given
foo!
bar!
