--TEST--
Using the same function name on interface with inheritance
--FILE--
<?php

interface Itest {
	function a();
}

interface Itest2 {
	function a();
}

interface Itest3 extends Itest, Itest2 {
}

?>
--EXPECTF--
Fatal error: Can't inherit abstract function Itest2::a() (previously declared abstract in Itest) in %s on line %d
