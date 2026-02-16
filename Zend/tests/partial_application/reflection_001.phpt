--TEST--
PFA reflection: required parameters
--FILE--
<?php
function foo($a = 1, $b = 5, $c = 10) {

}

$foo = foo(?, ...);

echo (string) new ReflectionFunction($foo);

$foo = foo(?, ?, ...);

echo (string) new ReflectionFunction($foo);

$foo = foo(?, ?, ?);

echo (string) new ReflectionFunction($foo);
?>
--EXPECTF--
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sreflection_001.php 6 - 6

  - Parameters [3] {
    Parameter #0 [ <optional> $a = 1 ]
    Parameter #1 [ <optional> $b = 5 ]
    Parameter #2 [ <optional> $c = 10 ]
  }
}
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sreflection_001.php 10 - 10

  - Parameters [3] {
    Parameter #0 [ <optional> $a = 1 ]
    Parameter #1 [ <optional> $b = 5 ]
    Parameter #2 [ <optional> $c = 10 ]
  }
}
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sreflection_001.php 14 - 14

  - Parameters [3] {
    Parameter #0 [ <optional> $a = 1 ]
    Parameter #1 [ <optional> $b = 5 ]
    Parameter #2 [ <optional> $c = 10 ]
  }
}
