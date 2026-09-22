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

echo "# Case 1\n";

$c = foo(b: ?);

echo (string) new ReflectionFunction($c);

$c(new B);

echo "# Case 2\n";

$c = $c(?);

echo (string) new ReflectionFunction($c);

$c(new B);

echo "# Case 3\n";

$c = foo(?, ?);
try {
    $c = $c(b: ?);
} catch (\Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

echo "# Case 4\n";

function bar($a = 1, $b = 2, ...$c) {
    var_dump($a, $b, $c);
}

$d = bar(b: ?, ...);

echo (string) new ReflectionFunction($d);

$d(new B, new A, new C);

echo "# Case 5\n";

try {
    $d = bar(?, a: ?);
} catch (\Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

echo "# Case 6\n";

try {
    $d = bar(c: ?, ...);
} catch (\Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECTF--
# Case 1
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %snamed_placeholders.php 13 - 13

  - Parameters [1] {
    Parameter #0 [ <required> $b ]
  }
}
int(1)
object(B)#%d (0) {
}
int(3)
# Case 2
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %snamed_placeholders.php 21 - 21

  - Bound Variables [1] {
      Variable #0 [ $fn ]
  }

  - Parameters [1] {
    Parameter #0 [ <required> $b ]
  }
}
int(1)
object(B)#%d (0) {
}
int(3)
# Case 3
ArgumentCountError: {closure:pfa:%s:29}(): Argument #1 ($a) not passed
# Case 4
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %snamed_placeholders.php 42 - 42

  - Parameters [3] {
    Parameter #0 [ <required> $b ]
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
# Case 5
Error: Named parameter $a overwrites previous placeholder
# Case 6
Error: Cannot use named placeholder for unknown or variadic parameter $c
