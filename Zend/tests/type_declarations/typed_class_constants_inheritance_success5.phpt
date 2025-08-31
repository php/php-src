--TEST--
Typed class constants (redefinition; interfaces and traits)
--FILE--
<?php
enum E {
    case Case1;
}

interface I {
    public const E CONST1 = E::Case1;
}

trait T {
    public const E CONST1 = E::Case1;
}

class C implements I {
    use T;

    public const E CONST1 = E::Case1;
}

var_dump(C::CONST1);
?>
--EXPECT--
enum(E::Case1)
