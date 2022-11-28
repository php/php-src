--TEST--
Capturing multiple Exceptions during autoloading
--FILE--
<?php

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

try {
    function_exists('foo');
} catch (Exception $e) {
    do {
        echo $e->getMessage()."\n";
    } while ($e = $e->getPrevious());
}

try {
    foo();
} catch (Exception $e) {
    do {
        echo $e->getMessage()."\n";
    } while ($e = $e->getPrevious());
}

function_exists('foo');
?>
===DONE===
--EXPECTF--
autoload_first
first
autoload_first
first
autoload_first

Fatal error: Uncaught Exception: first in %s:%d
Stack trace:
#0 [internal function]: autoload_first('foo')
#1 %s(%d): function_exists('foo')
#2 {main}
  thrown in %s on line %d
