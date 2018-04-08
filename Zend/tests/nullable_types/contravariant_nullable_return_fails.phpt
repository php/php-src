--TEST--
Return type cannot add nullability (contravariance)

--FILE--
<?php

interface A {
    function method(): int;
}

interface B extends A {
    function method(): ?int;
}

--EXPECTF--
Fatal error: Declaration of B::method(): ?int must be compatible with A::method(): int in %s on line %d

