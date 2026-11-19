--TEST--
ReflectionTargetAwareAttribute attribute for class constant
--FILE--
<?php

#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {

	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
		echo $target;
	}
}

class WithDemo {

	#[Demo("class constant")]
	public const EXAMPLE = true;

}

$case = new ReflectionClassConstant(WithDemo::class, 'EXAMPLE');
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECT--
Constant [ public bool EXAMPLE ] { true }

Constant [ public bool EXAMPLE ] { true }
