--TEST--
Lazy objects: initialization of proxy does not change the class of the object
--FILE--
<?php


class A {
    public string $s;
}
class B extends A {
    public function foo() {
        var_dump(__METHOD__);
    }
}

$reflector = new ReflectionClass(B::class);
$o = $reflector->newLazyProxy(function (B $o) {
    return new A();
});

var_dump(get_class($o));
$o->foo();
$o->s = 'init';
var_dump(get_class($o));
$o->foo();


?>
--EXPECT--
string(1) "B"
string(6) "B::foo"
string(1) "B"
string(6) "B::foo"
