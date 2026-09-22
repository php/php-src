--TEST--
When ArrayObject wraps an object, we should use prop table canonicalization
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
--EXPECTF--
Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
object(stdClass)#1 (1) {
  ["0"]=>
  int(1)
}
object(stdClass)#1 (1) {
  ["0"]=>
  int(2)
}
bool(true)
array(1) {
  [0]=>
  int(2)
}
object(stdClass)#1 (0) {
}
