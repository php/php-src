--TEST--
errmsg: __clone() cannot accept any arguments
--FILE--
<?php

class test {
	function __clone($var) {
	}
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Method test::__clone() cannot accept any arguments in %s on line %d
