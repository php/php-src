--TEST--
get_object_vars() - ensure references are preserved
--FILE--
<?php
$obj = new stdClass;
var_dump(get_object_vars($obj));

$a='original.a';
$obj->ref = &$a;
$obj->val = $a;

$arr = get_object_vars($obj);
var_dump($arr);

$arr['ref'] = 'changed.ref';
$arr['val'] = 'changed.val';

var_dump($arr, $obj, $a);
?>
--EXPECT--
array(0) {
}
array(2) {
  ["ref"]=>
  &string(10) "original.a"
  ["val"]=>
  string(10) "original.a"
}
array(2) {
  ["ref"]=>
  &string(11) "changed.ref"
  ["val"]=>
  string(11) "changed.val"
}
object(stdClass)#1 (2) {
  ["ref"]=>
  &string(11) "changed.ref"
  ["val"]=>
  string(10) "original.a"
}
string(11) "changed.ref"
