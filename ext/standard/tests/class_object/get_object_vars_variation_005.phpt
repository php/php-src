--TEST--
get_object_vars() no-declared/declared discrepancies
--FILE--
<?php

#[AllowDynamicProperties]
class Test {
    public $prop;
}
// Using ArrayObject here to get around property name restrictions

$obj = new stdClass;
$ao = new ArrayObject($obj);
$ao[12] = 6;
var_dump(get_object_vars($obj));

$obj = new Test;
$ao = new ArrayObject($obj);
$ao[12] = 6;
var_dump(get_object_vars($obj));

?>
--EXPECT--
array(1) {
  [12]=>
  int(6)
}
array(2) {
  ["prop"]=>
  NULL
  [12]=>
  int(6)
}
