--TEST--
ReflectionTargetAwareAttribute: constructor throws
--FILE--
<?php

#[Attribute]
class Demo implements ReflectionTargetAwareAttribute {

	public function __construct() {
		throw new Exception("testing");
	}

	public function setAttributeTarget(ReflectionAttributeTarget $target): void {
		echo __METHOD__ . "\n";
	}
}

#[Demo("global constant")]
const GLOBAL_CONSTANT = true;

$case = new ReflectionConstant('GLOBAL_CONSTANT');
echo $case;
echo "\n";
try {
	$case->getAttributes()[0]->newInstance();
} catch (Throwable $e) {
	echo get_class($e) . ': ' . $e->getMessage() . "\n";
}

?>
--EXPECT--
Constant [ bool GLOBAL_CONSTANT ] { true }

Exception: testing
