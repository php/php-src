--TEST--
self return type on closure in a method
--FILE--
<?php

class A {
    public function test() {
        return function() : self {
            return $this;
        };
    }
}

var_dump((new A)->test()());

?>
--EXPECT--
object(A)#1 (0) {
}
