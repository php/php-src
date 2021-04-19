--TEST--
Partial application magic: __callStatic
--FILE--
<?php
class Foo {
    public static function __callStatic($method, $args) {
        printf("%s::%s\n", __CLASS__, $method);
    }
}

$bar = Foo::method(?);

echo (string) new ReflectionFunction($bar);

$bar(1);
?>
--EXPECTF--
Method [ <user, prototype Foo> partial static public method method ] {
  @@ %s 8 - 8

  - Parameters [1] {
    Parameter #0 [ <required> $args ]
  }
}
Foo::method
