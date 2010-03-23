--TEST--
Bug #42107 (sscanf() broken when using %2$s type format parameters)
--FILE--
<?php

var_dump(sscanf('one two', '%1$s %2$s'));
var_dump(sscanf('one two', '%2$s %1$s'));
echo "--\n";
sscanf('one two', '%1$s %2$s', $foo, $bar);
var_dump($foo, $bar);
sscanf('one two', '%2$s %1$s', $foo, $bar);
var_dump($foo, $bar);
echo "--\n";
var_dump(sscanf('one two', '%1$d %2$d'));
var_dump(sscanf('one two', '%1$d'));
echo "Done\n";
?>
--EXPECTF--
array(2) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
}
array(2) {
  [0]=>
  string(3) "two"
  [1]=>
  string(3) "one"
}
--
string(3) "one"
string(3) "two"
string(3) "two"
string(3) "one"
--
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
Done
