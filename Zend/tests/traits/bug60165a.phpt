--TEST--
Bug #60165 (Aliasing unexisting trait should throw/trigger the exception/error)
--FILE--
<?php

trait A {
    public function bar() {}
}

class MyClass {
    use A {
        nonExistent as barA;
    }
}
--EXPECTF--
Fatal error: An alias (barA) was defined for method nonExistent(), but this method does not exist in %s on line %d
