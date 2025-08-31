--TEST--
SPL: Test class_implements() function : basic
--FILE--
<?php
echo "*** Testing class_implements() : basic ***\n";


interface foo { }
class bar implements foo {}

var_dump(class_implements(new bar));
var_dump(class_implements('bar'));


?>
--EXPECT--
*** Testing class_implements() : basic ***
array(1) {
  ["foo"]=>
  string(3) "foo"
}
array(1) {
  ["foo"]=>
  string(3) "foo"
}
