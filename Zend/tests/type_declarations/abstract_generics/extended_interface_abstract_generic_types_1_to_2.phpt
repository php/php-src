--TEST--
Abstract generic type behaviour in extended interface 2 generic type to 1
--FILE--
<?php

interface I<T1, T2> {
    public function foo(T1 $param): T2;
}

interface I2<S> extends I<S, S> {
    public function bar(int $o, S $param): S;
}

class C implements I2<string> {
    public function foo(string $param): string {}
    public function bar(int $o, string $param): string {}
}

?>
DONE
--EXPECT--
DONE
