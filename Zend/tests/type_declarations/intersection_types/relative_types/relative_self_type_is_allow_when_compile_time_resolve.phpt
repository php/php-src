--TEST--
self type can take part in an intersection type is resolvable at compile time
--FILE--
<?php

class A {
    public function foo(): self&Iterator {}
}

?>
DONE
--EXPECT--
DONE
