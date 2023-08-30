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
$ao["\0A\0b"] = 42;
$ao["\0*\0b"] = 24;
$ao[12] = 6;
var_dump(get_object_vars($obj));

$obj = new Test;
$ao = new ArrayObject($obj);
$ao["\0A\0b"] = 42;
$ao["\0*\0b"] = 24;
$ao[12] = 6;
var_dump(get_object_vars($obj));

?>
--EXPECTF--
array(3) {
  ["%0A%0b"]=>
  int(42)
  ["%0*%0b"]=>
  int(24)
  [12]=>
  int(6)
}
array(4) {
  ["prop"]=>
  NULL
  ["%0A%0b"]=>
  int(42)
  ["%0*%0b"]=>
  int(24)
  [12]=>
  int(6)
}
