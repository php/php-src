--TEST--
#[\Deprecated]: Class alias - call a static method
--FILE--
<?php

#[ClassAlias('MyAlias', [new Deprecated()])]
class Clazz {
	public static function doNothing() {
		echo __METHOD__ . "\n";
	}
}

MyAlias::doNothing();

?>
--EXPECTF--
Deprecated: Alias is deprecated in %s on line %d
Clazz::doNothing
