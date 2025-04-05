--TEST--
Abstract generic type behaviour in extended interface which redeclares method generic type covariant
--FILE--
<?php

interface I<T> {
    public function foo(int $param): T;
}

interface J<S> extends I<S> {
    public function foo(int $param): S;
}

?>
DONE
--EXPECT--
DONE

