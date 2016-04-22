--TEST--
union test return array or Traversable
--FILE--
<?php
$cb = function($i) : array | Traversable {
	return $i == 0 ? [] : new ArrayObject;
};

var_dump($cb(0), $cb(1));
?>
--EXPECT--
array(0) {
}
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
