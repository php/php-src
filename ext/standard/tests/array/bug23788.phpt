--TEST--
Bug #23788 (*_replace() clobbers referenced array elements)
--FILE--
<?php
$numeric = 123;
$bool = true;
$foo = array(&$numeric, &$bool);
var_dump($foo);
str_replace("abc", "def", $foo);
var_dump($foo);
?>
--EXPECT--
array(2) {
  [0]=>
  &int(123)
  [1]=>
  &bool(true)
}
array(2) {
  [0]=>
  &int(123)
  [1]=>
  &bool(true)
}
