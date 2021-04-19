--TEST--
Partial application variation variadics user
--FILE--
<?php
function foo($a, ...$b) {
    var_dump($a, ...$b);
}

$foo = foo(10, 100, ...);

echo (string) new ReflectionFunction($foo);

$foo(1000, 10000);
?>
--EXPECTF--
Function [ <user> partial function foo ] {
  @@ %s 6 - 6

  - Parameters [1] {
    Parameter #0 [ <optional> ...$b ]
  }
}
int(10)
int(100)
int(1000)
int(10000)
