--TEST--
Testing __callstatic declaration in interface with missing the 'static' modifier
--FILE--
<?php

interface a {
	function __callstatic($a, $b);
}

?>
--EXPECTF--
Warning: The magic method __callStatic() must have public visibility and be static in %s on line %d
