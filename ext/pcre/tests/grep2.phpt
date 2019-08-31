--TEST--
preg_grep() 2nd test
--SKIPIF--
<?php if (!PCRE_JIT_SUPPORT) die("skip no pcre jit support"); ?>
--INI--
pcre.jit=1
--FILE--
<?php

var_dump(preg_grep('/+/', array()));

$array = array(5=>'a', 'x' => '1', 'xyz'=>'q6', 'h20');

var_dump(preg_grep('@^[a-z]+@', $array));
var_dump(preg_grep('@^[a-z]+@', $array, PREG_GREP_INVERT));

ini_set('pcre.recursion_limit', 1);
var_dump(preg_last_error() == PREG_NO_ERROR);
var_dump(preg_grep('@^[a-z]+@', $array));
var_dump(preg_last_error() == PREG_RECURSION_LIMIT_ERROR);

?>
--EXPECTF--
Warning: preg_grep(): Compilation failed: quantifier does not follow a repeatable item at offset 0 in %sgrep2.php on line %d
bool(false)
array(3) {
  [5]=>
  string(1) "a"
  ["xyz"]=>
  string(2) "q6"
  [6]=>
  string(3) "h20"
}
array(1) {
  ["x"]=>
  string(1) "1"
}
bool(true)
array(3) {
  [5]=>
  string(1) "a"
  ["xyz"]=>
  string(2) "q6"
  [6]=>
  string(3) "h20"
}
bool(false)
