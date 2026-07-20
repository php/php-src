--TEST--
Constant argument optimization
--DESCRIPTION--
Pre-bound arguments that are constant can be burned into the generated
op_array instead of being passed via the Closure's lexical vars.
--ENV--
A=1
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php

function f($a, $b) {
    var_dump([$a, $b]);
}

function g() {
    return 3;
}

function h($a, $b, $c) {
    var_dump([$a, $b, $c]);
}

function i($a1, $a2, $a3, $a4, $a5, $a6, $a7, $a8, $a9, $a10, $a11, $a12, $a13, $a14, $a15, $a16, $a17, $a18, $a19, $a20, $a21, $a22, $a23, $a24, $a25, $a26, $a27, $a28, $a29, $a30, $a31, $a32, $a33) {
    var_dump($a33);
}

function print_lexical_vars($f) {
    $vars = new ReflectionFunction($f)->getClosureUsedVariables();
    if ($vars === []) {
        echo "no lexical vars\n";
    } else {
        $varNames = array_keys($vars);
        echo 'lexical vars: ', implode(', ', $varNames), "\n";
    }
}

echo "# Non-constant pre-bound argument:\n";
$f = f(getenv('A'), ?);
print_lexical_vars($f);
$f(2);

echo "# Constant pre-bound argument:\n";
$f = f(2, ?);
print_lexical_vars($f);
$f(2);

echo "# Constant pre-bound argument (inverted):\n";
$f = f(?, 2);
print_lexical_vars($f);
$f(2);

echo "# Inlined pre-bound argument:\n";
$f = f(g(), ?);
print_lexical_vars($f);
$f(2);

echo "# Constexpr pre-bound argument:\n";
const B = 4;
$f = f(B, ?);
print_lexical_vars($f);
$f(2);

echo "# Mixed arguments:\n";
$f = h(5, getenv('A'), ?);
print_lexical_vars($f);
$f(2);

echo "# Constant array pre-bound argument:\n";
$f = f(['foo' => 'bar'], ?);
print_lexical_vars($f);
$f(2);

echo "# Named arguments:\n";
$f = h(1, c: ?, b: ?);
print_lexical_vars($f);
$f(2, 3);

echo "# Many arguments (optimization can not be applied for all args) :\n";
$f = i(?, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33);
print_lexical_vars($f);
$f(33);

?>
--EXPECT--
# Non-constant pre-bound argument:
lexical vars: a
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  int(2)
}
# Constant pre-bound argument:
no lexical vars
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(2)
}
# Constant pre-bound argument (inverted):
no lexical vars
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(2)
}
# Inlined pre-bound argument:
no lexical vars
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(2)
}
# Constexpr pre-bound argument:
lexical vars: a
array(2) {
  [0]=>
  int(4)
  [1]=>
  int(2)
}
# Mixed arguments:
lexical vars: b
array(3) {
  [0]=>
  int(5)
  [1]=>
  string(1) "1"
  [2]=>
  int(2)
}
# Constant array pre-bound argument:
no lexical vars
array(2) {
  [0]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
  [1]=>
  int(2)
}
# Named arguments:
no lexical vars
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(3)
  [2]=>
  int(2)
}
# Many arguments (optimization can not be applied for all args) :
lexical vars: a33
int(33)
