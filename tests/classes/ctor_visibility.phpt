--TEST--
ZE2 A private constructor cannot be called
--FILE--
<?php

class Test
{
    function __construct()
    {
        echo __METHOD__ . "()\n";
    }
}

class Derived extends Test
{
	function __construct()
	{
        echo __METHOD__ . "()\n";
		parent::__construct();
	}
	
	static function f()
	{
		new Derived;
	}
}

Derived::f();

class TestPriv
{
    private function __construct()
    {
        echo __METHOD__ . "()\n";
    }

	static function f()
	{
		new TestPriv;
	}
}

TestPriv::f();

class DerivedPriv extends TestPriv
{
	function __construct()
	{
        echo __METHOD__ . "()\n";
		parent::__construct();
	}
	
	static function f()
	{
		new DerivedPriv;
	}
}

DerivedPriv::f();

?>
===DONE===
--EXPECTF--
Derived::__construct()
Test::__construct()
TestPriv::__construct()
DerivedPriv::__construct()

Fatal error: Cannot call private TestPriv::__construct() in %sctor_visibility.php on line %d
