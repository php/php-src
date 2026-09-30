--TEST--
Interface delegation: inherited class
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

class ParentClass implements A by $a {
    public function __construct(protected A $a) {}
}

class C extends ParentClass {}

echo (new C(new AImpl))->foo(), "\n";
?>

--EXPECT--
A
