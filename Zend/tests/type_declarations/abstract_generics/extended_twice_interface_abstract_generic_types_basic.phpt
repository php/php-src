--TEST--
Abstract generic type behaviour in extended interface
--FILE--
<?php

interface I1<T1> {
    public function foo(T1 $param): T1;
}

interface I2<T2> extends I1<T2> {
    public function bar(int $o, T2 $param): T2;
}

interface I3<T3> extends I2<T3> {
    public function foobar(T3 $a, float $b): float;
}

class C implements I3<string> {
    public function foo(string $param): string {}
    public function bar(int $o, string $param): string {}
    public function foobar(string $a, float $b): float {}
}

?>
DONE
--EXPECT--
DONE



Fatal error: CE: I3, Iface: I2, binding to Iface: I1, Generic type name: T2
Binding generic types for CE: I2 for interface: I1
Binding generic types for CE: I3 for interface: I2
Binding generic types for CE: I3 for inherited interface: I1
Binding generic types to generic type: T2 for inherited interface: I1
