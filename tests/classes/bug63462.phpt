--TEST--
Test script to verify that magic methods should be called only once when accessing an unset property.
--CREDITS--
Marco Pivetta <ocramius@gmail.com>
--XFAIL--
Bug 63462 is not yet fixed
--FILE--
<?php
class Test {
	public    $publicProperty;
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
		echo '__get ' . $name . "\n";
		return $this->$name;
	}

	function __set($name, $value) {
		echo '__set ' . $name . "\n";
		$this->$name = $value;
	}

	function __isset($name) {
		echo '__isset ' . $name . "\n";
		return isset($this->$name);
	}
}

$test = new Test();

$test->nonExisting;
$test->publicProperty;
$test->protectedProperty;
$test->privateProperty;
isset($test->nonExisting);
isset($test->publicProperty);
isset($test->protectedProperty);
isset($test->privateProperty);
$test->nonExisting       = 'value';
$test->publicProperty	 = 'value';
$test->protectedProperty = 'value';
$test->privateProperty   = 'value';

?>

--EXPECTF--
__get nonExisting
Notice: Undefined index: nonExisting in %__set__get_006.php on line %d
__get publicProperty
Notice: Undefined index: publicProperty in %__set__get_006.php on line %d
__get protectedProperty
Notice: Undefined index: protectedProperty in %__set__get_006.php on line %d
__get privateProperty
Notice: Undefined index: privateProperty in %__set__get_006.php on line %d
__isset nonExisting
__isset publicProperty
__isset protectedProperty
__isset privateProperty
__set nonExisting
__set publicProperty
__set protectedProperty
__set privateProperty
