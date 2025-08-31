--TEST--
SPL: Test class_implements() function : basic
--FILE--
<?php
echo "*** Testing class_uses() : basic ***\n";


trait foo { }
class bar { use foo; }

var_dump(class_uses(new bar));
var_dump(class_uses('bar'));


?>
--EXPECT--
*** Testing class_uses() : basic ***
array(1) {
  ["foo"]=>
  string(3) "foo"
}
array(1) {
  ["foo"]=>
  string(3) "foo"
}
