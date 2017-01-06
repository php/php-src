--TEST--
Bug #70982 (setStaticPropertyValue behaviors inconsistently with 5.6)
--FILE--
<?php
class Foo {
	static $abc;
	function __construct()
	{
		var_dump(self::$abc);
	}
}

class Bar extends Foo {

}

$rf = new ReflectionClass('Bar');
$rf->setStaticPropertyValue('abc', 'hi');
$foo = $rf->newInstance();
?>
--EXPECT--
string(2) "hi"
