--TEST--
Function default argument is not cached when its evaluation had side effects
--FILE--
<?php

class Foo {
    public function __toString() {
        static $i = 0;
        return (string) $i++;
    }
}

function test(string $foo = new Foo() . '') {
    var_dump($foo);
}

test();
test();
test();

?>
--EXPECT--
string(1) "0"
string(1) "1"
string(1) "2"
