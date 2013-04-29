--TEST--
preg_filter()
--FILE--
<?php

$subject = array('1', 'a', '2', 'b', '3', 'A', 'B', '4');
$pattern = array('/\d/', '/[a-z]/', '/[1a]/');
$replace = array('A:$0', 'B:$0', 'C:$0');

var_dump(preg_filter($pattern, $replace, $subject));

?>
===DONE===
--EXPECT--
array(6) {
  [0]=>
  string(5) "A:C:1"
  [1]=>
  string(5) "B:C:a"
  [2]=>
  string(3) "A:2"
  [3]=>
  string(3) "B:b"
  [4]=>
  string(3) "A:3"
  [7]=>
  string(3) "A:4"
}
===DONE===
