--TEST--
Behavior of static variables in parent trait method
--FILE--
<?php

trait T {
    public function foo()
    {
        var_dump(__CLASS__);

        static $v = null;
        if ($v === null) {
            $v = random_bytes(64);
        }
        
        return $v;
    }
}

class A { use T; }
class B extends A {}
class C extends A { use T; }
class D { use T; }

$a = (new A())->foo();
var_dump($a === (new A())->foo());
var_dump($a === (new B())->foo());
$c = new C();
var_dump($a === $c->foo());
var_dump($a === (new \ReflectionMethod(A::class, 'foo'))->invoke($c));
var_dump($a === (new D())->foo());

?>
--EXPECT--
string(1) "A"
string(1) "A"
bool(true)
string(1) "A"
bool(true)
string(1) "C"
bool(false)
string(1) "A"
bool(true)
string(1) "D"
bool(false)
