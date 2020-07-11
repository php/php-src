--TEST--
Bug #70179 ($this refcount issue)
--FILE--
<?php

class X {
    function __invoke() {
        var_dump($this);
    }
}
(new X)();

?>
--EXPECT--
object(X)#1 (0) {
}
