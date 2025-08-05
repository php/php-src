--TEST--
__call() can be partially applied
--FILE--
<?php
class Foo {
    public function __call($method, $args) {
        printf("%s::%s\n", __CLASS__, $method);

        var_dump(...$args);
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

try {
    $bar(1, 2);
} catch (Error $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}

$bar(1);

$bar = $foo->method(?, ...);

echo (string) new ReflectionFunction($bar);

$bar(10);

$bar = $foo->method(new Foo, ...);

echo (string) new ReflectionFunction($bar);

$bar(100);
?>
--EXPECTF--
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s 12 - 12

  - Parameters [1] {
    Parameter #0 [ <required> $args0 ]
  }
}
ArgumentCountError: Too few arguments to function Foo::{closure:%s:%d}(), 0 passed in %s on line %d and exactly 1 expected
Foo::method
int(1)
Foo::method
int(1)
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s 30 - 30

  - Parameters [2] {
    Parameter #0 [ <required> $args0 ]
    Parameter #1 [ <optional> ...$args ]
  }
}
Foo::method
int(10)
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %s 36 - 36

  - Bound Variables [1] {
      Variable #0 [ $args0 ]
  }

  - Parameters [1] {
    Parameter #0 [ <optional> ...$args ]
  }
}
Foo::method
object(Foo)#%d (0) {
}
int(100)
