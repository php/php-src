--TEST--
errmsg: __unset() must take exactly 1 argument
--FILE--
<?php

class test {
	function __unset() {
	}
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Method test::__unset() must take exactly 1 argument in %s on line %d
