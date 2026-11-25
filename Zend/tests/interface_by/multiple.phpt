--TEST--
Interface delegation: multiple delegated interfaces
--FILE--

<?php

interface A {
    public function foo(): string;
}

interface B {
    public function bar(): string;
}

class AImpl implements A {
    public function foo(): string {
        return 'A';
    }
}

class BImpl implements B {
    public function bar(): string {
        return 'B';
    }
}

class C implements A by $a, B by $b {
    public function __construct(
        protected A $a,
        protected B $b,
    ) {}
}

$c = new C(new AImpl, new BImpl);

echo $c->foo(), "\n";
echo $c->bar(), "\n";
?>

--EXPECT--
A
B
