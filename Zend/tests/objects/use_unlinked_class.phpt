--TEST--
Classes can only be used once they are fully linked
--FILE--
<?php

spl_autoload_register(function($class) {
    echo new ReflectionClass(A::class), "\n";
});

class A implements I {
}

?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "A" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->__construct('A')
#1 %s(%d): {closure:%s:%d}('I')
#2 {main}
  thrown in %s on line %d
