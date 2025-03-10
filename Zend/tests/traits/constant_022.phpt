--TEST--
Conflicts between trait constants and interface constants trigger fatal errors
--FILE--
<?php

trait MyTrait {
    const A = 1;
}
interface I {
    const A = 2;
}

class D implements I {
    use MyTrait;
}

echo D::A;
--EXPECTF--
Fatal error: I and MyTrait define the same constant (A) in the composition of D. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
