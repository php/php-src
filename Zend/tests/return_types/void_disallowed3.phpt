--TEST--
void return type: unacceptable cases: explicit null return in a method
--FILE--
<?php

class Foo {
    public function bar(): void {
        return -1; // not permitted in a void function
    }
}

?>
--EXPECTF--
Fatal error: A void method must not return a value in %s on line %d
