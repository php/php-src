--TEST--
get_class_constants(): Simple test
--FILE--
<?php

class test {
	const val = "string";
	const val2 = 1;
}
var_dump(get_class_constants('test'));
var_dump(get_class_constants(new test()));
?>
--EXPECT--	
array(2) {
  ["val"]=>
  string(6) "string"
  ["val2"]=>
  int(1)
}
array(2) {
  ["val"]=>
  string(6) "string"
  ["val2"]=>
  int(1)
}
