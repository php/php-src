--TEST--
#[\DelayedTargetValidation] with #[\Attribute]: invalid targets don't error
--FILE--
<?php

class NonAttribute {}

#[DelayedTargetValidation]
#[Attribute] // Does something here
class DemoClass {
	#[DelayedTargetValidation]
	#[Attribute] // Does nothing here
	public $val;
	
	public string $hooked {
		#[DelayedTargetValidation]
		#[Attribute] // Does nothing here
		get => $this->hooked;
		#[DelayedTargetValidation]
		#[Attribute] // Does nothing here
		set => $value;
	}

	#[DelayedTargetValidation]
	#[Attribute] // Does nothing here
	public const CLASS_CONST = 'FOO';

	public function __construct(
		#[DelayedTargetValidation]
		#[Attribute] // Does nothing here
		$str
	) {
		echo "Got: $str\n";
		$this->val = $str;
	}

	#[DelayedTargetValidation]
	#[Attribute] // Does nothing here
	public function printVal() {
		echo 'Value is: ' . $this->val . "\n";
	}

}

#[DelayedTargetValidation]
#[Attribute] // Does nothing here
function demoFn() {
	echo __FUNCTION__ . "\n";
	return 456;
}

#[DelayedTargetValidation]
#[Attribute] // Does nothing here
const GLOBAL_CONST = 'BAR';

$d = new DemoClass('example');
$d->printVal();
var_dump($d->val);
$d->hooked = "foo";
var_dump($d->hooked);
var_dump(DemoClass::CLASS_CONST);
demoFn();
var_dump(GLOBAL_CONST);

#[DemoClass('BAZ')]
#[NonAttribute]
class WithDemoAttribs {}

$ref = new ReflectionClass(WithDemoAttribs::class);
$attribs = $ref->getAttributes();
var_dump($attribs[0]->newInstance());
var_dump($attribs[1]->newInstance());

?>
--EXPECTF--
Got: example
Value is: example
string(7) "example"
string(3) "foo"
string(3) "FOO"
demoFn
string(3) "BAR"
Got: BAZ
object(DemoClass)#5 (1) {
  ["val"]=>
  string(3) "BAZ"
  ["hooked"]=>
  uninitialized(string)
}

Fatal error: Uncaught Error: Attempting to use non-attribute class "NonAttribute" as attribute in %s:%d
Stack trace:
#0 %s(%d): ReflectionAttribute->newInstance()
#1 {main}
  thrown in %s on line %d
