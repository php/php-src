--TEST--
errmsg: __destruct() cannot be static
--FILE--
<?php

class test {

	static function __destruct() {
	}
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Destructor test::__destruct() cannot be static in %s on line %d
