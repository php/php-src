--TEST--
Partial application magic: __call
--FILE--
<?php
class Foo {
    public function __call($method, $args) {
        printf("%s::%s\n", __CLASS__, $method);
    }
}

$foo = new Foo;

$bar = $foo->method(?);

echo (string) new ReflectionFunction($bar);

$bar(1);
?>
--EXPECTF--
Method [ <user, prototype Foo> partial public method method ] {
  @@ %s 10 - 10

  - Parameters [1] {
    Parameter #0 [ <required> $args ]
  }
}
Foo::method
