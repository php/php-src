--TEST--
Abstract generic type behaviour in extended interface
--FILE--
<?php

interface I<T1> {
    public function foo(T1 $param): T1;
}

interface I2<T2> extends I<T2> {
    public function bar(int $o, T2 $param): T2;
}

class C implements I2<string> {
    public function foo(string $param): string {}
    public function bar(int $o, string $param): string {}
}

?>
DONE
--EXPECT--
DONE
