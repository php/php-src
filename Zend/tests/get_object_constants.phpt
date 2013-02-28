--TEST--
get_object_constants(): Simple test
--FILE--
<?php

class test {
	const val = "string";
	const val2 = 1;
}
$test = new test();
var_dump(get_object_constants($test));
?>
--EXPECT--	
array(2) {
  ["val"]=>
  string(6) "string"
  ["val2"]=>
  int(1)
}
