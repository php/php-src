--TEST--
Yield can be used in nested method calls
--FILE--
<?php

class A {
    function foo($arg) {
        echo "Called A::foo\n";
    }
}

class B {
    function foo($arg) {
        echo "Called B::foo\n";
    }
}

function gen($obj) {
    $obj->foo($obj->foo(yield));
}

$g1 = gen(new A);
$g1->current();

$g2 = gen(new B);
$g2->current();

$g1->next();
$g2->next();

?>
--EXPECT--
Called A::foo
Called A::foo
Called B::foo
Called B::foo
