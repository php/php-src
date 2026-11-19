--TEST--
Interface delegation: parent and child delegations
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

class ParentClass implements A by $a {
    public function __construct(protected A $a) {}
}

class C extends ParentClass implements B by $b {
    public function __construct(
        A $a,
        protected B $b,
    ) {
        parent::__construct($a);
    }
}

$c = new C(new AImpl, new BImpl);

echo $c->foo(), "\n";
echo $c->bar(), "\n";
?>

--EXPECT--
A
B
