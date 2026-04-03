--TEST--
Implicit interface inheritance with same bound types
--FILE--
<?php

interface I1<T> {
    public function foo(T $param): T;
}

interface I2<T> extends I1<T> {
    public function bar(int $o, T $param): T;
}

class C implements I2<string>, I1<string> {
    public function foo(string $param): string {}
    public function bar(int $o, string $param): string {}
}

?>
DONE
--EXPECT--
DONE
