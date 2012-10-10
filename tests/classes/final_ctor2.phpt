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

Fatal error: Cannot override final Base::Base() with Extended::__construct() in %sfinal_ctor2.php on line %d
