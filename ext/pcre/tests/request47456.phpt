--TEST--
Request #47456 (Missing PCRE option 'J')
--DESCRIPTION--
The J modifier is supposed to be identical to the internal option (?J), so we're
testing both.
--FILE--
<?php
preg_match_all('/(?J)(?<chr>[ac])(?<num>\d)|(?<chr>[b])/', 'a1bc3', $m, PREG_SET_ORDER);
var_dump($m);

unset($m);
preg_match_all('/(?<chr>[ac])(?<num>\d)|(?<chr>[b])/J', 'a1bc3', $m, PREG_SET_ORDER);
var_dump($m);
?>
--EXPECT--
array(3) {
  [0]=>
  array(5) {
    [0]=>
    string(2) "a1"
    ["chr"]=>
    string(1) "a"
    [1]=>
    string(1) "a"
    ["num"]=>
    string(1) "1"
    [2]=>
    string(1) "1"
  }
  [1]=>
  array(6) {
    [0]=>
    string(1) "b"
    ["chr"]=>
    string(1) "b"
    [1]=>
    string(0) ""
    ["num"]=>
    string(0) ""
    [2]=>
    string(0) ""
    [3]=>
    string(1) "b"
  }
  [2]=>
  array(5) {
    [0]=>
    string(2) "c3"
    ["chr"]=>
    string(1) "c"
    [1]=>
    string(1) "c"
    ["num"]=>
    string(1) "3"
    [2]=>
    string(1) "3"
  }
}
array(3) {
  [0]=>
  array(5) {
    [0]=>
    string(2) "a1"
    ["chr"]=>
    string(1) "a"
    [1]=>
    string(1) "a"
    ["num"]=>
    string(1) "1"
    [2]=>
    string(1) "1"
  }
  [1]=>
  array(6) {
    [0]=>
    string(1) "b"
    ["chr"]=>
    string(1) "b"
    [1]=>
    string(0) ""
    ["num"]=>
    string(0) ""
    [2]=>
    string(0) ""
    [3]=>
    string(1) "b"
  }
  [2]=>
  array(5) {
    [0]=>
    string(2) "c3"
    ["chr"]=>
    string(1) "c"
    [1]=>
    string(1) "c"
    ["num"]=>
    string(1) "3"
    [2]=>
    string(1) "3"
  }
}
