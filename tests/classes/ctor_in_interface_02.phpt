--TEST--
ZE2 A class constructor must keep the signature of all interfaces
--FILE--
<?php
interface constr1
{
	function __construct();
}

interface constr2 extends constr1
{
}

class implem12 implements constr2
{
	function __construct()
	{
	}
}

interface constr3
{
	function __construct($a);
}

class implem13 implements constr1, constr3
{
	function __construct()
	{
	}
}

?>
--EXPECTF--
Fatal error: Can't inherit abstract function constr3::__construct() (previously declared abstract in constr1) in %s on line %d
