--TEST--
setAttributeTarget() is not called with the exact same target instance
--FILE--
<?php


#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {

	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
		global $case;
		assert($case !== $target);
    var_dump($case, $target);
	}
}

#[Demo("foo", true, 123)]
class WithDemo {}

$case = new ReflectionClass(WithDemo::class);
$originalInstance = $case->getAttributes()[0];
$originalInstance->newInstance();

?>
--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(8) "WithDemo"
}
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(8) "WithDemo"
}
