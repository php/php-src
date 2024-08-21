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

autoload_register_class('autoload_first');
autoload_register_class('autoload_second');

try {
    class_exists('ThisClassDoesNotExist');
} catch(Exception $e) {
    do {
        echo $e->getMessage()."\n";
    } while($e = $e->getPrevious());
}

try {
    new ThisClassDoesNotExist;
} catch(Exception $e) {
    do {
        echo $e->getMessage()."\n";
    } while($e = $e->getPrevious());
}

class_exists('ThisClassDoesNotExist');
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
#0 [internal function]: autoload_first('ThisClassDoesNo...')
#1 %s(%d): class_exists('ThisClassDoesNo...')
#2 {main}
  thrown in %s on line %d
