--TEST--
Closure 024: Trying to clone the Closure object
--FILE--
<?php

$a = function () { 
	return clone function () {
		return 1;
	};
};

$a();

?>
--EXPECTF--
Fatal error: Trying to clone an uncloneable object of class Closure in %s on line %d
