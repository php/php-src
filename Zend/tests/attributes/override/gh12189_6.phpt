--TEST--
#[Override]: Inheritance check of inherited trait method against interface
--FILE--
<?php

trait T1 {
    public abstract function test();
}

trait T2 {
    public function test() {}
}

class A {
    use T2;
}

class B extends A {
    use T1;
}

?>
===DONE===
--EXPECT--
===DONE===
