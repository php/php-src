--TEST--
Valid inheritence - co-variance
--FILE--
<?php

interface A {}
interface B {}
interface C {}

class Test implements A, B, C {}

class Foo {
    public function foo(): A {
        return new Test();
    }
}

class FooChild extends Foo {
    public function foo(): A&B {
        return new Test();
    }
}

class FooSecondChild extends FooChild {
    public function foo(): A&B&C {
        return new Test();
    }
}

$o = new FooChild();
var_dump($o->foo());
$o = new FooSecondChild();
var_dump($o->foo());

?>
--EXPECTF--
object(Test)#%d (0) {
}
object(Test)#%d (0) {
}
