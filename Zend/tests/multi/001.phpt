--TEST--
union test param array or Traversable
--FILE--
<?php
$cb = function(array | Traversable $thing) {
	return $thing;
};

var_dump($cb([]), $cb(new ArrayObject));
?>
--EXPECT--
array(0) {
}
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
