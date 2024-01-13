--TEST--
never return type: unacceptable cases: empty return in a method
--FILE--
<?php

class Foo {
    public function bar(): never {
        return;
    }
}

?>
--EXPECTF--
Fatal error: A never-returning method must not return in %s on line %d
