--TEST--
Bug #61780 (Inconsistent PCRE captures in match results): basics
--FILE--
<?php
preg_match('/(a)?([a-z]*)(\d*)/', '123', $matches, PREG_UNMATCHED_AS_NULL);
var_dump($matches);
?>
--EXPECT--
array(4) {
  [0]=>
  string(3) "123"
  [1]=>
  NULL
  [2]=>
  string(0) ""
  [3]=>
  string(3) "123"
}
