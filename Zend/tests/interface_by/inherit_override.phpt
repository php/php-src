--TEST--
Interface delegation: inherited concrete implementation wins
--FILE--

<?php

interface A {
    public function foo(): string;
}

class AImpl implements A {
    public function foo(): string {
        return 'delegate';
    }
}

class ParentClass {
    public function foo(): string {
        return 'parent';
    }
}

class C extends ParentClass implements A by $a {
    public function __construct(protected A $a) {}
}

echo (new C(new AImpl))->foo(), "\n";
?>

--EXPECT--
parent
