--TEST--
Reflection: ReflectionMethod::isGeneric()
--FILE--
<?php
class C {
    public function gen<T>(): void {}
    public function plain(): void {}
}
var_dump((new ReflectionClass('C'))->getMethod('gen')->isGeneric());
var_dump((new ReflectionClass('C'))->getMethod('plain')->isGeneric());
?>
--EXPECT--
bool(true)
bool(false)
