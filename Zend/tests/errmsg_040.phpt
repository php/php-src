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
Fatal error: Arrays are not allowed in constants at run-time in %s on line %d
