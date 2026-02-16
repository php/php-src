--TEST--
__call() can be partially applied
--FILE--
<?php
class Foo {
    public function __call($method, $arguments) {
        printf("%s::%s\n", __CLASS__, $method);

        var_dump(...$arguments);
    }
}

$foo = new Foo;

$bar = $foo->method(?);

echo (string) new ReflectionFunction($bar);

try {
    $bar();
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

$bar(1, 2);
$bar(1);

$bar = $foo->method(?, ...);

echo (string) new ReflectionFunction($bar);

$bar(10);
$bar(10, 20);

$bar = $foo->method(new Foo, ...);

echo (string) new ReflectionFunction($bar);

$bar(100);
?>
--EXPECTF--
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s 12 - 12

  - Parameters [1] {
    Parameter #0 [ <required> mixed $arguments0 ]
  }
}
ArgumentCountError: Too few arguments to function Foo::{closure:%s:%d}(), 0 passed in %s on line %d and exactly 1 expected
Foo::method
int(1)
Foo::method
int(1)
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s 25 - 25

  - Parameters [2] {
    Parameter #0 [ <required> mixed $arguments0 ]
    Parameter #1 [ <optional> mixed ...$arguments ]
  }
}
Foo::method
int(10)
Foo::method
int(10)
int(20)
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s 32 - 32

  - Bound Variables [1] {
      Variable #0 [ $arguments0 ]
  }

  - Parameters [1] {
    Parameter #0 [ <optional> mixed ...$arguments ]
  }
}
Foo::method
object(Foo)#%d (0) {
}
int(100)
