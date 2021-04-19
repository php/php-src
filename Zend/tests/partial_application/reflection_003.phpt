--TEST--
Partial application reflection: internal with variadics
--FILE--
<?php
$foo = sprintf(?);

echo (string) new ReflectionFunction($foo);

$foo = sprintf(?, ...);

echo (string) new ReflectionFunction($foo);

$foo = sprintf(?, ?);

echo (string) new ReflectionFunction($foo);
?>
--EXPECTF--
Function [ <internal:standard> partial function sprintf ] {

  - Parameters [1] {
    Parameter #0 [ <required> string $format ]
  }
  - Return [ string ]
}
Function [ <internal:standard> partial function sprintf ] {

  - Parameters [2] {
    Parameter #0 [ <required> string $format ]
    Parameter #1 [ <optional> mixed ...$values ]
  }
  - Return [ string ]
}
Function [ <internal:standard> partial function sprintf ] {

  - Parameters [2] {
    Parameter #0 [ <required> string $format ]
    Parameter #1 [ <required> mixed $values ]
  }
  - Return [ string ]
}

