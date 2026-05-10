--TEST--
Parametric LSP: calling parent::method() from substituted child still routes to the erased parent
--FILE--
<?php
class C<T> {
    public function f(T $x): void {
        echo "C::f got ", get_debug_type($x), "\n";
    }
}

class IntC extends C<int> {
    public function f(int $x): void {
        echo "IntC::f got $x\n";
        parent::f($x);
    }
}

(new IntC)->f(3);
?>
--EXPECT--
IntC::f got 3
C::f got int
