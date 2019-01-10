--TEST--
Bug #77439: parse_str segfaults when inserting item into existing array
--FILE--
<?php
$a = [];
parse_str('a[1]=1');
var_dump($a);
?>
--EXPECTF--
Deprecated: parse_str(): Calling parse_str() without the result argument is deprecated in %s on line %d
array(1) {
  [1]=>
  string(1) "1"
}
