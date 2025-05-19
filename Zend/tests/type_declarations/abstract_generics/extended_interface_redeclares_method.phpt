--TEST--
Abstract generic type behaviour in extended interface which redeclares method
--FILE--
<?php

interface I1<T> {
    public function foo(T $param): int;
}

interface I2<S> extends I1<S> {
    public function foo(S $param): int;
}

?>
DONE
--EXPECT--
DONE

