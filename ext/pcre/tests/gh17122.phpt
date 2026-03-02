--TEST--
GH-17122 (memory leak in regex)
--FILE--
<?php
preg_match('|(?P<name>)(\d+)|', 0xffffffff, $m1);
var_dump($m1);
\preg_match('|(?P<name2>)(\d+)|', 0, $m2);
var_dump($m2);
?>
--EXPECT--
array(4) {
  [0]=>
  string(10) "4294967295"
  ["name"]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(10) "4294967295"
}
array(4) {
  [0]=>
  string(1) "0"
  ["name2"]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(1) "0"
}
