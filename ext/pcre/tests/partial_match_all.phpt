--TEST--
Partial matching can be used with preg_match_all() in PREG_SET_ORDER
--FILE--
<?php

$str = "abc abc ab";
var_dump(preg_match_all("/abc/P", $str));
echo "\n";

var_dump(preg_match_all("/abc/P", $str, $matches, PREG_SET_ORDER));
var_dump(preg_last_error() == PREG_PARTIAL_MATCH_ERROR);
var_dump($matches);

?>
--EXPECTF--
Warning: preg_match_all(): Partial matching in preg_match_all() can only be used with PREG_SET_ORDER in %s on line %d
NULL

int(3)
bool(true)
array(3) {
  [0]=>
  array(1) {
    [0]=>
    string(3) "abc"
  }
  [1]=>
  array(1) {
    [0]=>
    string(3) "abc"
  }
  [2]=>
  array(2) {
    [0]=>
    string(2) "ab"
    [1]=>
    string(2) "ab"
  }
}
