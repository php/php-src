--TEST--
#[\DelayedTargetValidation] with #[\Override]: non-overrides still error (class constant)
--FILE--
<?php

class DemoClass {

	#[DelayedTargetValidation]
	#[Override] // Does something here
	public const CLASS_CONSTANT = 'FOO';
}

?>
--EXPECTF--
Fatal error: DemoClass::CLASS_CONSTANT has #[\Override] attribute, but no matching parent constant exists in %s on line %d
