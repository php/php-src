--TEST--
errmsg: static abstract function 
--FILE--
<?php

class test {
	static abstract function foo ();
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Static function test::foo() cannot be abstract in %s on line %d
