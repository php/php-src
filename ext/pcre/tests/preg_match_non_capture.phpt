--TEST--
testing /n modifier in preg_match()
--FILE--
<?php

preg_match('/.(.)./n', 'abc', $m);
var_dump($m);

preg_match('/.(?P<test>.)./n', 'abc', $m);
var_dump($m);

?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "abc"
}
array(3) {
  [0]=>
  string(3) "abc"
  ["test"]=>
  string(1) "b"
  [1]=>
  string(1) "b"
}
