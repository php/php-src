--TEST--
Typed class constants (incompatible composition; traits)
--FILE--
<?php

trait T {
    public const ?array CONST1 = [];
}

class C {
    use T;

    public const CONST1 = [];
}

?>
--EXPECTF--
Fatal error: C and T define the same constant (CONST1) in the composition of C. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
