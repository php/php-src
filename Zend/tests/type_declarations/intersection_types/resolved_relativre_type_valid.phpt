--TEST--
parent type cannot take part in an intersection type
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
