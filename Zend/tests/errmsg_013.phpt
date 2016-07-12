--TEST--
errmsg: default value for parameters with array type can only be an array or NULL
--FILE--
<?php

class test {
	function foo(array $a = "s") {
	}
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Default value for parameters with array type can only be an array or NULL in %s on line %d
