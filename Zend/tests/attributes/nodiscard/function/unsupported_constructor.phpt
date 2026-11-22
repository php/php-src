--TEST--
#[\NoDiscard]: Not allowed on '__construct'.
--FILE--
<?php

class Clazz {
	#[\NoDiscard]
	public function __construct() {
	}
}

$cls = new Clazz();

?>
--EXPECTF--
Fatal error: Method Clazz::__construct cannot be #[\NoDiscard] in %s on line %d
