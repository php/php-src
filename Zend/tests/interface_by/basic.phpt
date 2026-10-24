--TEST--
Interface delegation: basic method call
--FILE--

<?php

interface A {
    public function foo(): string;
}

class AImpl implements A {
    public function foo(): string {
        return 'A';
    }
}

class C implements A by $a {
    public function __construct(protected A $a) {}
}

echo (new C(new AImpl))->foo(), "\n";
?>

--EXPECT--
A
