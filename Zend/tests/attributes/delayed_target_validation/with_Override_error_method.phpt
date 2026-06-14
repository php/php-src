--TEST--
#[\DelayedTargetValidation] with #[\Override]: non-overrides still error (method)
--FILE--
<?php

class DemoClass {

	#[DelayedTargetValidation]
	#[Override] // Does something here
	public function printVal() {
		echo 'Value is: ' . $this->val . "\n";
		return 123;
	}
}

?>
--EXPECTF--
Fatal error: DemoClass::printVal() has #[\Override] attribute, but no matching parent method exists in %s on line %d
