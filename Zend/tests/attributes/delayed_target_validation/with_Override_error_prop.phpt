--TEST--
#[\DelayedTargetValidation] with #[\Override]: non-overrides still error (normal property)
--FILE--
<?php

class DemoClass {

	#[DelayedTargetValidation]
	#[Override] // Does something here
	public string $prop;
}

?>
--EXPECTF--
Fatal error: DemoClass::$prop has #[\Override] attribute, but no matching parent property exists in %s on line %d
