--TEST--
errmsg: arrays are not allowed in class constants
--FILE--
<?php

class test {
	const TEST = array(1,2,3);
}
var_dump(test::TEST);
echo "Done\n";
?>
--EXPECTF--	
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
Done
