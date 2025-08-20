--TEST--
#[\DelayedTargetValidation] with #[\Override]: non-overrides still error (get hook)
--FILE--
<?php

class DemoClass {

	public string $hooked {
		#[DelayedTargetValidation]
		#[Override] // Does something here
		get => $this->hooked;
		set => $value;
	}
}

?>
--EXPECTF--
Fatal error: DemoClass::$hooked::get() has #[\Override] attribute, but no matching parent method exists in %s on line %d
