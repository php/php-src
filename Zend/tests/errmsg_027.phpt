--TEST--
errmsg: class declarations may not be nested
--FILE--
<?php

class test {
	function foo() {
		class test2 {
		}
	}
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Class declarations may not be nested in %s on line %d
