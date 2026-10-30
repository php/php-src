--TEST--
Bug #75079 variation without traits
--FILE--
<?php

class Foo
{
    private static $bar = 123;

    static function test(){
        return function(){
            return function(){
                return Foo::$bar;
            };
        };
    }
}


$f = Foo::test();

var_dump($f()());

class A{}
$a = new A;
var_dump($f->bindTo($a, A::CLASS)()());

?>
--EXPECTF--
int(123)

Fatal error: Uncaught Error: Cannot access private property Foo::$bar in %s:%d
Stack trace:
#0 %s(%d): A->{closure:%s:%d}()
#1 {main}
  thrown in %s on line %d
