--TEST--
Bug #60165 (Aliasing unexisting trait should throw/trigger the exception/error)
--FILE--
<?php

trait A {
    public function bar() {}
}

trait B {
    public function foo() {}
}

class MyClass {
    use A, B {
        foo as fooB;
        baz as foobar;
    }
}

--EXPECTF--
Fatal error: An alias (foobar) was defined for method baz(), but this method does not exist in %s on line %d
