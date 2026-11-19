--TEST--
ReflectionTargetAwareAttribute attribute for property
--FILE--
<?php

#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {

	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
		echo $target;
	}
}

class WithDemo {

	#[Demo("property")]
	public mixed $prop;

}

$case = new ReflectionProperty(WithDemo::class, 'prop');
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECT--
Property [ public mixed $prop ]

Property [ public mixed $prop ]
