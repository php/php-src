--TEST--
GH-8932 (Wrong closure scope class reported for static methods)
--FILE--
<?php
class A {
    public static function b() {
        echo static::class;
    }
}

class B extends A {}

$c = ['B', 'b'];
$d = \Closure::fromCallable($c);
$r = new \ReflectionFunction($d);
var_dump($r->getClosureScopeClass());
$d();
?>
--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "B"
}
B
