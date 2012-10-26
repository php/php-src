--TEST--
ZE2 Tests that an auto-implemented getter has a protected auto-implemented variable defined and that it can be retrieved through the accessor
--FILE--
<?php

class AccessorTest {
	public $b {
		get;
	}

	public function __construct() {
		$this->__b = 5;
	}
}

$o = new AccessorTest();

$rf = new ReflectionClass($o);
foreach($rf->getProperties(ReflectionProperty::IS_PROTECTED) as $rfp) {
	if($rfp->getName() == '__b')
		echo "Protected property: \$".$rfp->getName()." exists.\n";
}

echo "\$o->b: ".$o->b."\n";
echo "Done\n";
?>
--EXPECTF--
Protected property: $__b exists.
$o->b: 5
Done
