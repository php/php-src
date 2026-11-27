--TEST--
#[\NoDiscard]: Not allowed on '__clone'.
--FILE--
<?php

class Clazz {
	#[\NoDiscard]
	public function __clone() {
	}
}

$cls = new Clazz();

?>
--EXPECTF--
Fatal error: Method Clazz::__clone cannot be #[\NoDiscard] in %s on line %d
