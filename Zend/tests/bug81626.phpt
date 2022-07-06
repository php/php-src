--TEST--
Bug #81626: Error on use static:: in __сallStatic() wrapped to Closure::fromCallable()
--FILE--
<?php
class TestClass {
    public static bool $wasCalled = false;
    public static function __callStatic(string $name, array $args): string
    {
        static::$wasCalled = true;
        return 'ok';
    }
}
$closure = Closure::fromCallable([TestClass::class, 'foo']);
var_dump($closure());
?>
--EXPECT--
string(2) "ok"
