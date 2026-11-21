--TEST--
ReflectionTargetAwareAttribute attribute for closure parameter
--FILE--
<?php

#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {

	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
		echo $target;
	}
}

$closure = static function (#[Demo("closure param")] mixed $param) {};

$case = new ReflectionParameter($closure, 'param');
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECT--
Parameter #0 [ <required> mixed $param ]
Parameter #0 [ <required> mixed $param ]
