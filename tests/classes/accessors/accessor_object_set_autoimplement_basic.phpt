--TEST--
ZE2 Tests that an auto-implemented setter has a protected auto-implemented variable defined and that it can be changed through the accessor
--FILE--
<?php

class AccessorTest {
	public $b {
		set;
	}

	public function __construct() {
		$this->__b = 5;
	}
	public function _getProtectedValue() { return $this->__b; }
}

$o = new AccessorTest();

$rf = new ReflectionClass($o);
foreach($rf->getProperties(ReflectionProperty::IS_PROTECTED) as $rfp) {
	if($rfp->getName() == '__b')
		echo "Protected property: \$".$rfp->getName()." exists.\n";
}

echo "\$o->b: ".$o->_getProtectedValue()."\n";
$o->b = 10;
echo "\$o->b: ".$o->_getProtectedValue()."\n";
echo "Done\n";
?>
--EXPECTF--
Protected property: $__b exists.
$o->b: 5
$o->b: 10
Done
