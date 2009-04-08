--TEST--
ReflectionClass::getExtensionName() method - variation test for getExtensionName()
--CREDITS--
Rein Velt <rein@velt.org>
#testFest Roosendaal 2008-05-10
--FILE--
<?php

	class myClass
	{	
		public $varX;
		public $varY;
	}
	$rc=new reflectionClass('myClass');
	var_dump( $rc->getExtensionName()) ;
?>
--EXPECT--
bool(false)
