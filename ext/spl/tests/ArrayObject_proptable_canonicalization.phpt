--TEST--
When ArrayObject wraps an object, we should use proptable canonicalization
--FILE--
<?php

$o = new stdClass;
$ao = new ArrayObject($o);
$ao[0] = 1;
var_dump($o);
$ao[0] += 1;
var_dump($o);
var_dump(isset($ao[0]));
var_dump((array) $ao);
unset($ao[0]);
var_dump($o);

?>
--EXPECT--
object(DynamicObject)#1 (1) {
  ["0"]=>
  int(1)
}
object(DynamicObject)#1 (1) {
  ["0"]=>
  int(2)
}
bool(true)
array(1) {
  [0]=>
  int(2)
}
object(DynamicObject)#1 (0) {
}
