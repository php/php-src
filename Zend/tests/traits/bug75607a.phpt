--TEST--
Bug #75607 (Comparison of initial static properties failing)
--FILE--
<?php

trait T1
{
	public static $prop1 = 1;
}

trait T2
{
	public static $prop1 = 1;
}

class Base
{
	use T1;
}

class Child extends base
{

}

class Grand extends Child
{
	use T2;
}

$c = new Grand();
var_dump($c::$prop1);

?>
--EXPECT--
int(1)
