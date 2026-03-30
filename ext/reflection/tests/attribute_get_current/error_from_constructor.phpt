--TEST--
ReflectionAttribute::getCurrent() when called from another method invoked by constructor
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct($args) {
		echo ReflectionAttribute::getCurrent();
		$this->test();
	}

	private function test() {
		echo ReflectionAttribute::getCurrent();
	}
}

#[Demo("global constant")]
const GLOBAL_CONSTANT = true;

$case = new ReflectionConstant('GLOBAL_CONSTANT');
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECTF--
Constant [ bool GLOBAL_CONSTANT ] { 1 }

Constant [ bool GLOBAL_CONSTANT ] { 1 }

Fatal error: Uncaught Error: Current function was not invoked via ReflectionAttribute::newInstance() in %s:%d
Stack trace:
#0 %s(%d): ReflectionAttribute::getCurrent()
#1 %s(%d): Demo->test()
#2 %s(%d): Demo->__construct('global constant')
#3 %s(%d): ReflectionAttribute->newInstance()
#4 {main}
  thrown in %s on line %d
