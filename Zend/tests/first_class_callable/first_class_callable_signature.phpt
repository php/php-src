--TEST--
First class callables should retain the signature for reflection
--FILE--
<?php

function test(int $a, string &$b, Foo... $c) {}

echo new ReflectionFunction(test(...));

?>
--EXPECTF--
Closure [ <user> function test ] {
  @@ %s 3 - 3

  - Parameters [3] {
    Parameter #0 [ <required> int $a ]
    Parameter #1 [ <required> string &$b ]
    Parameter #2 [ <optional> Foo ...$c ]
  }
}
