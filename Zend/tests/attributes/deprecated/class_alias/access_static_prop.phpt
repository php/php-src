--TEST--
#[\Deprecated]: Class alias - access a static variable
--FILE--
<?php

#[ClassAlias('MyAlias', [new Deprecated()])]
class Clazz {
	public static mixed $v = NULL;
}

MyAlias::$v = true;

?>
--EXPECTF--
Deprecated: Alias is deprecated in %s on line %d
