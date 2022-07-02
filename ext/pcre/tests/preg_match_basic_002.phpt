--TEST--
preg_match() single line match with multi-line input
--FILE--
<?php
$string = "My\nName\nIs\nStrange";
preg_match("/M(.*)/", $string, $matches);

var_dump($matches);
?>
--EXPECT--
array(2) {
  [0]=>
  string(2) "My"
  [1]=>
  string(1) "y"
}
