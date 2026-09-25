--TEST--
Constant argument optimization - coerced arg with '...' on non-variadic function
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--FILE--
<?php
function g(string $s, $x = 1) {
    var_dump($s, func_get_args());
}

$f = g(15, ...);
$f();
$f(2, 3);

?>
--EXPECT--
string(2) "15"
array(2) {
  [0]=>
  string(2) "15"
  [1]=>
  int(1)
}
string(2) "15"
array(3) {
  [0]=>
  string(2) "15"
  [1]=>
  int(2)
  [2]=>
  int(3)
}
