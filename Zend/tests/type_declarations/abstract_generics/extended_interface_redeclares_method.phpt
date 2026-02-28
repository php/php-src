--TEST--
Abstract generic type behaviour in extended interface which redeclares method
--FILE--
<?php

interface I<T> {
    public function foo(T $param): int;
}

interface J extends I<float> {
    public function foo(float $param): int;
}

?>
DONE
--EXPECT--
DONE

