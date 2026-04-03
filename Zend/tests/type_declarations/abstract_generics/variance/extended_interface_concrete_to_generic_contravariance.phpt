--TEST--
Redeclaring a method that has a concrete type into a generic type (contravariance)
--FILE--
<?php

interface I {
    public function foo(int $param): int;
}

interface J<S> extends I {
    public function foo(S $param): int;
}

?>
DONE
--EXPECTF--
Fatal error: Declaration of J<S : mixed>::foo(<S> $param): int must be compatible with I::foo(int $param): int %s on line %d
