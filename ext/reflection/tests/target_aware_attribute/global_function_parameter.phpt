--TEST--
ReflectionTargetAwareAttribute attribute for function parameter
--FILE--
<?php

#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {

	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
		echo $target;
	}
}

function globalFunc(
	#[Demo("function param")] mixed $param
) {}

$case = new ReflectionParameter('globalFunc', 'param');
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECT--
Parameter #0 [ <required> mixed $param ]
Parameter #0 [ <required> mixed $param ]
