--TEST--
Interface delegation: delegated and ordinary interfaces
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

class C implements A by $a, B {
    public function __construct(protected A $a) {}

    public function bar(): string {
        return 'B';
    }
}

$c = new C(new AImpl);

echo $c->foo(), "\n";
echo $c->bar(), "\n";
?>

--EXPECT--
A
B
