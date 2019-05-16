--TEST--
ZE2 __call() signature check
--FILE--
<?php

class Test {
	function __call() {
	}
}

?>
--EXPECTF--
Fatal error: Method Test::__call() must take exactly 2 arguments in %s__call_002.php on line %d
