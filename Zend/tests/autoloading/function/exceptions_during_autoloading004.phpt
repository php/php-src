--TEST--
Capturing multiple Exceptions during autoloading in a namespace
--FILE--
<?php
namespace {
    function autoload_first($name) {
      echo __METHOD__ . "\n";
      throw new Exception('first');
    }

    function autoload_second($name) {
      echo __METHOD__ . "\n";
      throw new Exception('second');
    }

    autoload_register_function('autoload_first');
    autoload_register_function('autoload_second');
}
namespace bar {
    echo "Try-catch around function_exists()\n";
    try {
        \function_exists('foo');
    } catch (\Exception $e) {
        do {
            echo $e->getMessage()."\n";
        } while ($e = $e->getPrevious());
    }
    echo "Try-catch around unqualified function call\n";
    try {
        foo();
    } catch (\Throwable $e) {
        do {
            echo $e::class, ': ', $e->getMessage(), "\n";
        } while ($e = $e->getPrevious());
    }
    echo "Try-catch around qualified function call\n";
    try {
        \foo();
    } catch (\Throwable $e) {
        do {
            echo $e::class, ': ', $e->getMessage(), "\n";
        } while ($e = $e->getPrevious());
    }

    echo "function_exists() without try-catch\n";
    \function_exists('foo');
}
?>
--EXPECTF--
Try-catch around function_exists()
autoload_first
first
Try-catch around unqualified function call
autoload_first
Exception: first
Try-catch around qualified function call
autoload_first
Exception: first
function_exists() without try-catch
autoload_first

Fatal error: Uncaught Exception: first in %s:%d
Stack trace:
#0 [internal function]: autoload_first('foo')
#1 %s(%d): function_exists('foo')
#2 {main}
  thrown in %s on line %d
