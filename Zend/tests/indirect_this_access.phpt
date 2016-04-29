--TEST--
Make sure variable-variable accesses are not marked as "using $this"
--FILE--
<?php

class A {
    public function method() {
        $name = 'this';
        var_dump($$name);
    }
}

(new A)->method();
A::method();

?>
--EXPECTF--
object(A)#1 (0) {
}

Deprecated: Non-static method A::method() should not be called statically in %s on line %d

Notice: Undefined variable: this in %s on line %d
NULL
