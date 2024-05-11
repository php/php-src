--TEST--
GH-12509: JIT assertion when running php-parser tests
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
function capture() {
    $GLOBALS["x"] = new Exception();
}
function test($a) {
    $a[1] = 1;
    $a[2] = 2;
    capture();
    $a[3] = 3;
    return $a;
}
var_dump(test([]));
?>
--EXPECT--
array(3) {
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}