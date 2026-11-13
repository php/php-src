--TEST--
Interface delegation: exception propagation
--FILE--

<?php

interface A {
    public function foo(): void;
}

class AImpl implements A {
    public function foo(): void {
        throw new Exception('delegated');
    }
}

class C implements A by $a {
    public function __construct(protected A $a) {}
}

try {
    (new C(new AImpl))->foo();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>

--EXPECT--
delegated
