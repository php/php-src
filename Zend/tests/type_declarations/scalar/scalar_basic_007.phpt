--TEST--
scalar return type covariance
--DESCRIPTION--
Tests that it is possible in subtypes to narrow down the constraint on return
types that their supertype defined.
--FILE--
<?php

         interface I { function f(): scalar; }
class A implements I { function f(): bool   { return true;  } }
class B implements I { function f(): float  { return 4.2;   } }
class C implements I { function f(): int    { return 42;    } }
class D implements I { function f(): string { return 'str'; } }

var_dump(
    (new A)->f(),
    (new B)->f(),
    (new C)->f(),
    (new D)->f()
);

?>
--EXPECT--
bool(true)
float(4.2)
int(42)
string(3) "str"
