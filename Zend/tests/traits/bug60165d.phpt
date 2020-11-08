--TEST--
Bug #60165 (Aliasing unexisting trait should throw/trigger the exception/error)
--FILE--
<?php

// The same is true for the insteadof operator to resolve conflicts

trait A {}

trait B {
    public function bar() {}
}

class MyClass {
    use A, B {
        A::bar insteadof B;
    }
}
?>
--EXPECTF--
Fatal error: A precedence rule was defined for A::bar but this method does not exist in %s on line %d
