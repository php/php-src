--TEST--
__compareTo: Should be called when comparing for equality
--FILE--
<?php
class A
{
    public function __compareTo($other)
    {
        echo "A::__compareTo\n";
    }
}

class B extends A
{
    public function __equals($other)
    {
        echo "B::__equals\n";
    }
}

new A == new A; // A::__compareTo
new A != new A; // A::__compareTo
new A === new A;
new A !== new A;

new B == new A; // B::__equals
new B != new A; // B::__equals
new B === new A;
new B !== new A;

new A == new B; // B::__equals
new A != new B; // B::__equals
new A === new B;
new A !== new B;

new B == new B; // B::__equals
new B != new B; // B::__equals
new B === new B;
new B !== new B;
?>
--EXPECT--
A::__compareTo
A::__compareTo
B::__equals
B::__equals
B::__equals
B::__equals
B::__equals
B::__equals
