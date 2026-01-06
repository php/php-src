--TEST--
__callStatic() can be partially applied
--FILE--
<?php
class Foo {
    public static function __callStatic($method, $arguments) {
        printf("%s::%s\n", __CLASS__, $method);

        var_dump(...$arguments);
    }
}

$bar = Foo::method(?);

echo (string) new ReflectionFunction($bar);

$bar(1);
$bar(1, 2);

$bar = Foo::method(?, ...);

echo (string) new ReflectionFunction($bar);

$bar(10);
$bar(10, 20);

$bar = Foo::method(new Foo,...);

echo (string) new ReflectionFunction($bar);

$bar(100);
?>
--EXPECTF--
Closure [ <user> static public method {closure:%s:%d} ] {
  @@ %s 10 - 10

  - Parameters [1] {
    Parameter #0 [ <required> mixed $arguments0 ]
  }
}
Foo::method
int(1)
Foo::method
int(1)
Closure [ <user> static public method {closure:%s:%d} ] {
  @@ %s 17 - 17

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
Closure [ <user> static public method {closure:%s:%d} ] {
  @@ %s 24 - 24

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
