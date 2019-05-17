--TEST--
ZE2 Initializing static properties to arrays
--FILE--
<?php

class test {
	static public $ar = array();
}

var_dump(test::$ar);

test::$ar[] = 1;

var_dump(test::$ar);

echo "Done\n";
?>
--EXPECT--
array(0) {
}
array(1) {
  [0]=>
  int(1)
}
Done
