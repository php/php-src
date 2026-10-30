--TEST--
Interface delegation: method arguments are forwarded
--FILE--

<?php

interface A {
    public function add(int $a, int $b = 10): int;
}

class AImpl implements A {
    public function add(int $a, int $b = 10): int {
        return $a + $b;
    }
}

class C implements A by $a {
    public function __construct(protected A $a) {}
}

$c = new C(new AImpl);

echo $c->add(2), "\n";
echo $c->add(2, 3), "\n";
?>

--EXPECT--
12
5
