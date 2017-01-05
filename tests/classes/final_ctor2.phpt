--TEST--
ZE2 cannot override final old style ctor
--FILE--
<?php

class Base
{
	public final function Base()
	{
	}
}

class Works extends Base
{
}

class Extended extends Base
{
	public function __construct()
	{
	}
}

ReflectionClass::export('Extended');

?>
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; Base has a deprecated constructor in %s on line %d

Fatal error: Cannot override final Base::Base() with Extended::__construct() in %sfinal_ctor2.php on line %d
