--TEST--
parent/self type can take part in an intersection type if it is resolvable at compile time
--FILE--
<?php

class A {}

class B extends A {
    public function foo(): parent&Iterator {}
}

class C extends A {
    public function foo(): self&Iterator {}
}

?>
==DONE==
--EXPECT--
==DONE==
