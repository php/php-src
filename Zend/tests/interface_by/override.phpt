--TEST--
Interface delegation: existing concrete method is not replaced
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

class C implements A by $a {
    public function __construct(protected A $a) {}

    public function foo(): string {
        return 'class';
    }
}

echo (new C(new AImpl))->foo(), "\n";
?>

--EXPECT--
class
