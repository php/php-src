--TEST--
Abstract generic type behaviour in extended interface which redeclares method but does not use bound generic type (covariance)
--FILE--
<?php

interface I<T> {
    public function foo(int $param): T;
}

interface J<S> extends I<S> {
    public function foo(int $param): int;
}

?>
DONE
--EXPECTF--
Fatal error: Declaration of J<S : mixed>::foo(int $param): int must be compatible with I<T : <S>>::foo(int $param): <T : <S>> in %s on line %d
