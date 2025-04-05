--TEST--
Abstract generic type behaviour in extended interface which redeclares method generic type contravariant
--FILE--
<?php

interface I<T> {
    public function foo(T $param): int;
}

interface J<S> extends I<S> {
    public function foo(S $param): int;
}

?>
DONE
--EXPECT--
DONE
