--TEST--
Verifies that ReflectionProperty does not cause magic methods to get called when properties are unset
--FILE--
<?php

class Test
{
	public	$publicProperty;
	protected $protectedProperty;
	private   $privateProperty;

	public function __construct() {
		unset(
			$this->publicProperty,
			$this->protectedProperty,
			$this->privateProperty
		);
	}

	function __get($name) {
		echo 'called __get ' . $name;
	}

	function __set($name, $value) {
		echo 'called __set ' . $name . ' ' . $value;
	}
}

$t = new Test();

foreach (array('publicProperty', 'protectedProperty', 'privateProperty') as $propertyName) {
	$prop = new ReflectionProperty('Test', $propertyName);
	$prop->setAccessible(true);
	echo var_export($prop->getValue($t), true) . "\n";
	$prop->setValue($t, 'value');
}

?>
--EXPECTF--
NULL
NULL
NULL
