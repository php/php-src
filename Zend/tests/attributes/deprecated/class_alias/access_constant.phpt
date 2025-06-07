--TEST--
#[\Deprecated]: Class alias - access a constant
--FILE--
<?php

#[ClassAlias('MyAlias', [new Deprecated()])]
class Clazz {
	public const TEST = 1;
}

var_dump(MyAlias::TEST);

?>
--EXPECTF--
Deprecated: Alias MyAlias for class Clazz is deprecated in %s on line %d
int(1)
