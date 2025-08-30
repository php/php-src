--TEST--
#[\DelayedTargetValidation] with #[\Deprecated]: valid targets are deprecated
--FILE--
<?php

#[DelayedTargetValidation]
#[Deprecated] // Does nothing here
class DemoClass {
	#[DelayedTargetValidation]
	#[Deprecated] // Does nothing here
	public $val;

	public string $hooked {
		#[DelayedTargetValidation]
		#[Deprecated] // Does something here
		get => $this->hooked;
		#[DelayedTargetValidation]
		#[Deprecated] // Does something here
		set => $value;
	}

	#[DelayedTargetValidation]
	#[Deprecated] // Does something here
	public const CLASS_CONST = 'FOO';

	public function __construct(
		#[DelayedTargetValidation]
		#[Deprecated] // Does nothing here
		$str
	) {
		echo "Got: $str\n";
		$this->val = $str;
	}

	#[DelayedTargetValidation]
	#[Deprecated] // Does something here
	public function printVal() {
		echo 'Value is: ' . $this->val . "\n";
		return 123;
	}
}

#[DelayedTargetValidation]
#[Deprecated] // Does something here
trait DeprecatedTrait {}

class WithDeprecatedTrait {
	use DeprecatedTrait;
}

#[DelayedTargetValidation]
#[Deprecated] // Does something here
function demoFn() {
	echo __FUNCTION__ . "\n";
	return 456;
}

#[DelayedTargetValidation]
#[Deprecated] // Does something here
const GLOBAL_CONST = 'BAR';

$d = new DemoClass('example');
$d->printVal();
var_dump($d->val);
$d->hooked = "foo";
var_dump($d->hooked);
var_dump(DemoClass::CLASS_CONST);
demoFn();
var_dump(GLOBAL_CONST);
?>
--EXPECTF--
Deprecated: Trait DeprecatedTrait used by WithDeprecatedTrait is deprecated in %s on line %d
Got: example

Deprecated: Method DemoClass::printVal() is deprecated in %s on line %d
Value is: example
string(7) "example"

Deprecated: Method DemoClass::$hooked::set() is deprecated in %s on line %d

Deprecated: Method DemoClass::$hooked::get() is deprecated in %s on line %d
string(3) "foo"

Deprecated: Constant DemoClass::CLASS_CONST is deprecated in %s on line %d
string(3) "FOO"

Deprecated: Function demoFn() is deprecated in %s on line %d
demoFn

Deprecated: Constant GLOBAL_CONST is deprecated in %s on line %d
string(3) "BAR"
