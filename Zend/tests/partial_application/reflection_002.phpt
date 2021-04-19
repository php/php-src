--TEST--
Partial application reflection: variadics
--FILE--
<?php
function foo($a, ...$b) {
    var_dump(func_get_args());
}

$foo = foo(?);

echo (string) new ReflectionFunction($foo);

$foo = foo(?, ...);

echo (string) new ReflectionFunction($foo);

$foo = foo(?, ?);

echo (string) new ReflectionFunction($foo);

$foo = foo(?, ?, ?);

echo (string) new ReflectionFunction($foo);
?>
--EXPECTF--
Function [ <user> partial function foo ] {
  @@ %s 6 - 6

  - Parameters [1] {
    Parameter #0 [ <required> $a ]
  }
}
Function [ <user> partial function foo ] {
  @@ %s 10 - 10

  - Parameters [2] {
    Parameter #0 [ <required> $a ]
    Parameter #1 [ <optional> ...$b ]
  }
}
Function [ <user> partial function foo ] {
  @@ %s 14 - 14

  - Parameters [2] {
    Parameter #0 [ <required> $a ]
    Parameter #1 [ <required> $b ]
  }
}
Function [ <user> partial function foo ] {
  @@ %s 18 - 18

  - Parameters [3] {
    Parameter #0 [ <required> $a ]
    Parameter #1 [ <required> $b ]
    Parameter #2 [ <required> $b ]
  }
}
