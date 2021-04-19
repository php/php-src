--TEST--
Partial application reflection: required parameters
--FILE--
<?php
function foo($a = 1, $b = 5, $c = 10) {

}

$foo = foo(...);

echo (string) new Reflectionfunction($foo);

$foo = foo(?, ...);

echo (string) new Reflectionfunction($foo);

$foo = foo(?, ?, ...);

echo (string) new ReflectionFunction($foo);

$foo = foo(?, ?, ?);

echo (string) new ReflectionFunction($foo);
?>
--EXPECTF--
Function [ <user> partial function foo ] {
  @@ %s 6 - 6

  - Parameters [3] {
    Parameter #0 [ <optional> $a = 1 ]
    Parameter #1 [ <optional> $b = 5 ]
    Parameter #2 [ <optional> $c = 10 ]
  }
}
Function [ <user> partial function foo ] {
  @@ %s 10 - 10

  - Parameters [3] {
    Parameter #0 [ <required> $a ]
    Parameter #1 [ <optional> $b = 5 ]
    Parameter #2 [ <optional> $c = 10 ]
  }
}
Function [ <user> partial function foo ] {
  @@ %s 14 - 14

  - Parameters [3] {
    Parameter #0 [ <required> $a ]
    Parameter #1 [ <required> $b ]
    Parameter #2 [ <optional> $c = 10 ]
  }
}
Function [ <user> partial function foo ] {
  @@ %s 18 - 18

  - Parameters [3] {
    Parameter #0 [ <required> $a ]
    Parameter #1 [ <required> $b ]
    Parameter #2 [ <required> $c ]
  }
}

