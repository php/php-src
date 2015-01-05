--TEST--
Bug #67633: A foreach on an array returned from a function not doing copy-on-write
--FILE--
<?php

function id($x) {
    return $x;
}

function &ref_id(&$x) {
    return $x;
}

$c = 'c';
$array = ['a', 'b', $c];

foreach(id($array) as &$v) {
    $v .= 'q';
}
var_dump($array);

foreach(ref_id($array) as &$v) {
    $v .= 'q';
}
var_dump($array);

?>
--EXPECT--
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
array(3) {
  [0]=>
  string(2) "aq"
  [1]=>
  string(2) "bq"
  [2]=>
  &string(2) "cq"
}
