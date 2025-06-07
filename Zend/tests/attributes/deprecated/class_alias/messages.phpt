--TEST--
#[\Deprecated]: Class alias - all messages have details
--FILE--
<?php

#[ClassAlias('MyAlias', [new Deprecated("don't use the alias", "8.4")])]
class Clazz {
	public const TEST = 1;

	public static mixed $v = NULL;

	public function __construct() {}

	public static function doNothing() {}
}

var_dump(MyAlias::TEST);
MyAlias::$v = true;

$o = new MyAlias();

MyAlias::doNothing();

?>
--EXPECTF--
Deprecated: Alias is deprecated since 8.4, don't use the alias in %s on line %d
int(1)

Deprecated: Alias is deprecated since 8.4, don't use the alias in %s on line %d

Deprecated: Alias is deprecated since 8.4, don't use the alias in %s on line %d

Deprecated: Alias is deprecated since 8.4, don't use the alias in %s on line %d
