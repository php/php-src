--TEST--
Bug #49908 (throwing exception in autoloader crashes when interface is not defined)
--FILE--
<?php

spl_autoload_register(function ($className) {
    var_dump($className);

    if ($className == 'Foo') {
        class Foo implements Bar {};
    } else {
        throw new Exception($className);
    }
});

try {
    new Foo();
} catch (Exception $e) { }

// We never reach here.
var_dump(new Foo());

?>
--EXPECTF--
string(3) "Foo"
string(3) "Bar"
string(3) "Foo"
string(3) "Bar"

Fatal error: Uncaught Exception: Bar in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}('Bar')
#1 %s(%d): {closure:%s:%d}('Foo')
#2 {main}
  thrown in %s on line %d
