--TEST--
Readonly property with default value satisfies get-only interface property
--FILE--
<?php

interface I {
    public int $prop { get; }
}

class C implements I {
    public readonly int $prop = 42;
}

var_dump(new C()->prop);
?>
--EXPECTF--
int(42)
