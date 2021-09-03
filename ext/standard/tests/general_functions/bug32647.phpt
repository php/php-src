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
    register_shutdown_function(array($obj,""));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    register_shutdown_function(array($obj,"some string"));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    register_shutdown_function(array(0,""));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    register_shutdown_function(array('bar','foo'));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    register_shutdown_function(array(0,"some string"));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    register_shutdown_function('bar');
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

register_shutdown_function('foo');

try {
    register_shutdown_function(array('bar','barfoo'));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

$obj = new bar;

try {
    register_shutdown_function(array($obj,'foobar'));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

register_shutdown_function(array($obj,'barfoo'));

?>
--EXPECTF--
Warning: Undefined variable $obj in %s on line %d
register_shutdown_function(): Argument #1 ($callback) must be a valid callback, first array member is not a valid class name or object

Warning: Undefined variable $obj in %s on line %d
register_shutdown_function(): Argument #1 ($callback) must be a valid callback, first array member is not a valid class name or object
register_shutdown_function(): Argument #1 ($callback) must be a valid callback, first array member is not a valid class name or object
register_shutdown_function(): Argument #1 ($callback) must be a valid callback, class bar does not have a method "foo"
register_shutdown_function(): Argument #1 ($callback) must be a valid callback, first array member is not a valid class name or object
register_shutdown_function(): Argument #1 ($callback) must be a valid callback, function "bar" not found or invalid function name
register_shutdown_function(): Argument #1 ($callback) must be a valid callback, non-static method bar::barfoo() cannot be called statically
register_shutdown_function(): Argument #1 ($callback) must be a valid callback, class bar does not have a method "foobar"
foo!
bar!
