--TEST--
#[\DelayedTargetValidation] prevents target errors at compile time
--FILE--
<?php

#[DelayedTargetValidation]
#[NoDiscard]
class Demo {

	#[DelayedTargetValidation]
	#[Attribute]
	public const FOO = 'BAR';

	#[DelayedTargetValidation]
	#[Attribute]
	public string $v1;

	public string $v2 {
		#[DelayedTargetValidation]
		#[Attribute]
		get => $this->v2;
		#[DelayedTargetValidation]
		#[Attribute]
		set => $value;
	}

	#[DelayedTargetValidation]
	#[Attribute]
	public function __construct(
		#[DelayedTargetValidation]
		#[Attribute]
		public string $v3
	) {
		$this->v1 = $v3;
		echo __METHOD__ . "\n";
	}
}

#[DelayedTargetValidation]
#[Attribute]
function demoFn() {
	echo __FUNCTION__ . "\n";
}

$o = new Demo( "foo" );
demoFn();

#[DelayedTargetValidation]
#[Attribute]
const EXAMPLE = true;

?>
--EXPECT--
Demo::__construct
demoFn
