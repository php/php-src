--TEST--
Interface delegation: multiple methods
--FILE--

<?php

interface A {
    public function foo(string $value): string;
    public function bar(int $value = 42): int;
}

class AImpl implements A {
    public function foo(string $value): string {
        return strtoupper($value);
    }

    public function bar(int $value = 42): int {
        return $value * 2;
    }
}

class C implements A by $a {
    public function __construct(protected A $a) {}
}

$c = new C(new AImpl);

echo $c->foo('hello'), "\n";
echo $c->bar(), "\n";
echo $c->bar(21), "\n";
?>

--EXPECT--
HELLO
84
42
