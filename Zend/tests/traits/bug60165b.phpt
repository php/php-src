--TEST--
Bug #60165 (Aliasing unexisting trait should throw/trigger the exception/error)
--FILE--
<?php

trait A {
    public function bar() {}
}

class MyClass {
    use A {
        A::nonExistent as barA;
    }
}
?>
--EXPECTF--
Fatal error: An alias was defined for A::nonExistent but this method does not exist in %s on line %d
