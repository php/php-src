--TEST--
Test closure isStatic
--FILE--
<?php
class Foo {
    public static function bar(){}
    public function baz() {}
    
    public function qux() {
        return static function(){};
    }
}

$foo = new Foo;

var_dump(
    (new ReflectionFunction(function(){}))->isStatic(),
    (new ReflectionFunction(static function(){}))->isStatic(),
    (new ReflectionFunction($foo->qux()))->isStatic(),
    (new ReflectionMethod($foo, 'bar'))->isStatic(),
    (new ReflectionMethod($foo, 'baz'))->isStatic(),);
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
