--TEST--
#[\Deprecated]: Class alias - use the constructor
--FILE--
<?php

#[ClassAlias('MyAlias', [new Deprecated()])]
class Clazz {
	public function __construct() {
		echo __METHOD__ . "\n";
	}
}

$o = new MyAlias();

?>
--EXPECTF--
Deprecated: Alias MyAlias for class Clazz is deprecated in %s on line %d
Clazz::__construct
