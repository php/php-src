--TEST--
Named parameters define the order of parameters in a PFA
--FILE--
<?php

function f($a, $b, $c, $d = null) {
    return [$a, $b, $c, $d];
}

function g($a, $b, $c, ...$d) {
    return [$a, $b, $c, $d];
}

echo "# All named\n";

$a = f(d: ?, c: ?, b: ?, a: ?);
echo new ReflectionFunction($a), "\n";
var_dump($a(1, 2, 3, 4));

echo "# Some named: Positional first, then named in specified order\n";

$a = f(?, ?, d: ?, c: ?);
echo new ReflectionFunction($a), "\n";
var_dump($a(1, 2, 3, 4));

echo "# Some named, one unspecified\n";

$a = f(?, c: ?, b: ?);
echo new ReflectionFunction($a), "\n";
var_dump($a(1, 2, 3, 4));

echo "# Some named, some implicit added by '...'\n";

$a = f(c: ?, b: ?, ...);
echo new ReflectionFunction($a), "\n";
var_dump($a(1, 2, 3, 4));

echo "# Some named, some implicit added by '...' on variadic function\n";

$a = g(c: ?, b: ?, ...);
echo new ReflectionFunction($a), "\n";
var_dump($a(1, 2, 3, 4, 5, 6));

echo "# Some prebound, some named\n";

$a = f(-1, c: ?, d: -2, b: ?);
echo new ReflectionFunction($a), "\n";
var_dump($a(1, 2));

echo "# Some named, some required missing\n";

try {
    $a = f(b: ?, c: ?);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECTF--
# All named
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sparam_reorder.php 13 - 13

  - Parameters [4] {
    Parameter #0 [ <required> $d ]
    Parameter #1 [ <required> $c ]
    Parameter #2 [ <required> $b ]
    Parameter #3 [ <required> $a ]
  }
}

array(4) {
  [0]=>
  int(4)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
}
# Some named: Positional first, then named in specified order
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sparam_reorder.php 19 - 19

  - Parameters [4] {
    Parameter #0 [ <required> $a ]
    Parameter #1 [ <required> $b ]
    Parameter #2 [ <required> $d ]
    Parameter #3 [ <required> $c ]
  }
}

array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(4)
  [3]=>
  int(3)
}
# Some named, one unspecified
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sparam_reorder.php 25 - 25

  - Parameters [3] {
    Parameter #0 [ <required> $a ]
    Parameter #1 [ <required> $c ]
    Parameter #2 [ <required> $b ]
  }
}

array(4) {
  [0]=>
  int(1)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  NULL
}
# Some named, some implicit added by '...'
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sparam_reorder.php 31 - 31

  - Parameters [4] {
    Parameter #0 [ <required> $c ]
    Parameter #1 [ <required> $b ]
    Parameter #2 [ <required> $a ]
    Parameter #3 [ <optional> $d = NULL ]
  }
}

array(4) {
  [0]=>
  int(3)
  [1]=>
  int(2)
  [2]=>
  int(1)
  [3]=>
  int(4)
}
# Some named, some implicit added by '...' on variadic function
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sparam_reorder.php 37 - 37

  - Parameters [4] {
    Parameter #0 [ <required> $c ]
    Parameter #1 [ <required> $b ]
    Parameter #2 [ <required> $a ]
    Parameter #3 [ <optional> ...$d ]
  }
}

array(4) {
  [0]=>
  int(3)
  [1]=>
  int(2)
  [2]=>
  int(1)
  [3]=>
  array(3) {
    [0]=>
    int(4)
    [1]=>
    int(5)
    [2]=>
    int(6)
  }
}
# Some prebound, some named
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %sparam_reorder.php 43 - 43

  - Bound Variables [2] {
      Variable #0 [ $a ]
      Variable #1 [ $d ]
  }

  - Parameters [2] {
    Parameter #0 [ <required> $c ]
    Parameter #1 [ <required> $b ]
  }
}

array(4) {
  [0]=>
  int(-1)
  [1]=>
  int(2)
  [2]=>
  int(1)
  [3]=>
  int(-2)
}
# Some named, some required missing
ArgumentCountError: f(): Argument #1 ($a) not passed
