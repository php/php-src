--TEST--
GH-8932 (Provide a way to get the called-scope of closures)
--FILE--
<?php
class A {
    public static function __callStatic($name, $args) {
        echo static::class.'::'.$name, "\n";
    }

    public function __call($name, $args) {
        echo static::class.'->'.$name, "\n";
    }

    public static function b() {
        echo static::class.'::b', "\n";
    }


    public function c() {
        echo static::class.'->c', "\n";
    }
}

class B extends A {}

$c = ['B', 'b'];
$d = \Closure::fromCallable($c);
$r = new \ReflectionFunction($d);
var_dump($r->getClosureCalledClass());
$d();

$c = [new B(), 'c'];
$d = \Closure::fromCallable($c);
$r = new \ReflectionFunction($d);
var_dump($r->getClosureCalledClass());
$d();

$c = ['B', 'd'];
$d = \Closure::fromCallable($c);
$r = new \ReflectionFunction($d);
var_dump($r->getClosureCalledClass());
$d();

$c = [new B(), 'e'];
$d = \Closure::fromCallable($c);
$r = new \ReflectionFunction($d);
var_dump($r->getClosureCalledClass());
$d();
?>
--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "B"
}
B::b
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "B"
}
B->c
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "B"
}
B::d
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(1) "B"
}
B->e
