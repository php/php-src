--TEST--
PFA supports named placeholders
--FILE--
<?php

class A {}
class B {}
class C {}

function foo($a = 1, $b = 2, $c = 3) {
    var_dump($a, $b, $c);
}

$foo = foo(b: ?);

echo (string) new ReflectionFunction($foo);

$foo(new B);

$foo = $foo(?);

echo (string) new ReflectionFunction($foo);

$foo(new B);

$foo = foo(?, ?);
$foo = $foo(b: ?);

echo (string) new ReflectionFunction($foo);

$foo(new B);

function bar($a = 1, $b = 2, ...$c) {
    var_dump($a, $b, $c);
}

$bar = bar(b: ?, ...);

echo (string) new ReflectionFunction($bar);

$bar(new B, new A, new C);

try {
    $bar = bar(?, a: ?);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $bar = bar(c: ?, ...);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %snamed_placeholders.php 11 - 11

  - Parameters [1] {
    Parameter #0 [ <optional> $b = 2 ]
  }
}
int(1)
object(B)#%d (0) {
}
int(3)
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %snamed_placeholders.php 17 - 17

  - Bound Variables [1] {
      Variable #0 [ $fn ]
  }

  - Parameters [1] {
    Parameter #0 [ <optional> $b = 2 ]
  }
}
int(1)
object(B)#%d (0) {
}
int(3)
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %snamed_placeholders.php 24 - 24

  - Bound Variables [1] {
      Variable #0 [ $fn ]
  }

  - Parameters [1] {
    Parameter #0 [ <optional> $b = 2 ]
  }
}
int(1)
object(B)#%d (0) {
}
int(3)
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %snamed_placeholders.php 34 - 34

  - Parameters [3] {
    Parameter #0 [ <optional> $b = 2 ]
    Parameter #1 [ <optional> $a = 1 ]
    Parameter #2 [ <optional> ...$c ]
  }
}
object(A)#%d (0) {
}
object(B)#%d (0) {
}
array(1) {
  [0]=>
  object(C)#%d (0) {
  }
}
Named parameter $a overwrites previous placeholder
Cannot use named placeholder for unknown or variadic parameter $c
