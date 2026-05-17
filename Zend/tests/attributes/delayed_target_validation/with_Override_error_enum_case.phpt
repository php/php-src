--TEST--
#[\DelayedTargetValidation] with #[\Override]: non-overrides still error (enum case)
--FILE--
<?php

enum DemoEnum {

	#[DelayedTargetValidation]
	#[Override] // Does something here
	case MyCase;
}

?>
--EXPECTF--
Fatal error: DemoEnum::MyCase has #[\Override] attribute, but no matching parent constant exists in %s on line %d
