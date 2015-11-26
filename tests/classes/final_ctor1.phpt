--TEST--
ZE2 cannot override final __construct
--FILE--
<?php

class Base
{
	public final function __construct()
	{
	}
}

class Works extends Base
{
}

class Extended extends Base
{
	public function Extended()
	{
	}
}

ReflectionClass::export('Extended');

?>
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; Extended has a deprecated constructor in %s on line %d

Fatal error: Cannot override final Base::__construct() with Extended::Extended() in %sfinal_ctor1.php on line %d
