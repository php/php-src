--TEST--
Foreach loop tests - using an array element as the $value
--FILE--
<?php

$a=array("a", "b", "c");
$v=array();
foreach($a as $v[0]) {
    var_dump($v);
}
var_dump($a);
var_dump($v);

echo "\n";
$a=array("a", "b", "c");
$v=array();
foreach($a as $k=>$v[0]) {
    var_dump($k, $v);
}
var_dump($a);
var_dump($k, $v);
?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "b"
}
array(1) {
  [0]=>
  string(1) "c"
}
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
array(1) {
  [0]=>
  string(1) "c"
}

int(0)
array(1) {
  [0]=>
  string(1) "a"
}
int(1)
array(1) {
  [0]=>
  string(1) "b"
}
int(2)
array(1) {
  [0]=>
  string(1) "c"
}
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
int(2)
array(1) {
  [0]=>
  string(1) "c"
}
