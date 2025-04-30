--TEST--
never return type: unacceptable cases: implicit return in a method
--FILE--
<?php

class Foo {
    public static function bar(): never {
        if (false) {
            throw new Exception('bad');
        }
    }
}

try {
    Foo::bar();
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Foo::bar(): never-returning method must not implicitly return
