--TEST--
Redeclaring a method that has a concrete type into a generic type (covariance)
--FILE--
<?php

interface I {
    public function foo(int $param): int;
}

interface J<S> extends I {
    public function foo(int $param): S;
}

?>
DONE
--EXPECTF--
Fatal error: Declaration of J<S : mixed>::foo(int $param): <S> must be compatible with I::foo(int $param): int %s on line %d
