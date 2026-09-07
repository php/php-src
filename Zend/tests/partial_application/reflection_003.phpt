--TEST--
PFA reflection: internal with variadics
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
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sreflection_003.php 2 - 2

  - Parameters [1] {
    Parameter #0 [ <required> string $format ]
  }
  - Return [ string ]
}
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sreflection_003.php 6 - 6

  - Parameters [2] {
    Parameter #0 [ <required> string $format ]
    Parameter #1 [ <optional> mixed ...$values ]
  }
  - Return [ string ]
}
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sreflection_003.php 10 - 10

  - Parameters [2] {
    Parameter #0 [ <required> string $format ]
    Parameter #1 [ <required> mixed $values0 ]
  }
  - Return [ string ]
}
