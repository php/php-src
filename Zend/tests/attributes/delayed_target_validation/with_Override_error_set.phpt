--TEST--
#[\DelayedTargetValidation] with #[\Override]: non-overrides still error (set hook)
--FILE--
<?php

class DemoClass {

	public string $hooked {
		get => $this->hooked;
		#[DelayedTargetValidation]
		#[Override] // Does something here
		set => $value;
	}
}

?>
--EXPECTF--
Fatal error: DemoClass::$hooked::set() has #[\Override] attribute, but no matching parent method exists in %s on line %d
