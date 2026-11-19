--TEST--
ReflectionTargetAwareAttribute attribute for global constant
--FILE--
<?php

#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {

	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
		echo $target;
	}
}

#[Demo("global constant")]
const GLOBAL_CONSTANT = true;

$case = new ReflectionConstant('GLOBAL_CONSTANT');
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECT--
Constant [ bool GLOBAL_CONSTANT ] { true }

Constant [ bool GLOBAL_CONSTANT ] { true }
