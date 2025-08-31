--TEST--
parent type can take part in an intersection type is resolvable at compile time
--FILE--
<?php

class A {}

class B extends A {
    public function foo(): parent&Iterator {}
}

?>
DONE
--EXPECT--
DONE
