--TEST--
GH-17376: Child classes may add hooks to plain properties
--FILE--
<?php

class A {
    public $prop = 1;
}

class B extends A {
    public $prop {
        get => 2;
    }
}

function test(A $a) {
    var_dump($a->prop);
}

test(new A);
test(new B);

?>
--EXPECT--
int(1)
int(2)
