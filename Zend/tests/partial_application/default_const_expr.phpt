--TEST--
PFA: constant-expression default for a skipped optional parameter
--FILE--
<?php

namespace App;

const K = 7;

function f($a, $b = K, $c = 0) {
    return [$a, $b, $c];
}

function variadic($a, $b = K, $c = 0, ...$args) {
    return [$a, $b, $c, $args];
}

function late($a, $b = LATE, $c = 0) {
    return [$a, $b, $c];
}

function fallback($a, $b = GLOBAL_ONLY, $c = 0) {
    return [$a, $b, $c];
}

class C {
    const X = 42;
    static function m($a, $b = self::X, $c = 0) {
        return [$a, $b, $c];
    }
}

enum E {
    case A;
    case B;
}

function g($a, $b = E::A, $c = 0) {
    return [$a, $b->name, $c];
}

$p = f(a: 10, c: ?);
var_dump($p(99));

$v = variadic(a: 10, c: ?, foo: 1);
var_dump($v(99));

$q = C::m(a: 1, c: ?);
var_dump($q(9));

$r = g(a: 1, c: ?);
var_dump($r(9));

$l = late(a: 1, c: ?);
define('App\LATE', 'defined after the partial was created');
var_dump($l(9));

define('GLOBAL_ONLY', 'global fallback');
$s = fallback(a: 1, c: ?);
var_dump($s(9));

?>
--EXPECT--
array(3) {
  [0]=>
  int(10)
  [1]=>
  int(7)
  [2]=>
  int(99)
}
array(4) {
  [0]=>
  int(10)
  [1]=>
  int(7)
  [2]=>
  int(99)
  [3]=>
  array(1) {
    ["foo"]=>
    int(1)
  }
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(42)
  [2]=>
  int(9)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  string(1) "A"
  [2]=>
  int(9)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  string(37) "defined after the partial was created"
  [2]=>
  int(9)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  string(15) "global fallback"
  [2]=>
  int(9)
}
