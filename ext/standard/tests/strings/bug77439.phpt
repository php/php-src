--TEST--
Bug #77439: parse_str segfaults when inserting item into existing array
--FILE--
<?php
$vars = ['a' => []];
parse_str('a[1]=1', $vars);
var_dump($vars['a']);
?>
--EXPECT--
array(1) {
  [1]=>
  string(1) "1"
}
