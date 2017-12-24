--TEST--
nullable scalar return type covariance
--FILE--
<?php

         interface I { function f(): ?scalar; }
class A implements I { function f(): ?bool   { return null; } }
class B implements I { function f(): ?float  { return null; } }
class C implements I { function f(): ?int    { return null; } }
class D implements I { function f(): ?string { return null; } }

var_dump(
    (new A)->f(),
    (new B)->f(),
    (new C)->f(),
    (new D)->f()
);

?>
--EXPECT--
NULL
NULL
NULL
NULL
