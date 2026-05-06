--TEST--
ReflectionAttribute::getCurrent() and ::getReflectionTarget() for closure parameter
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct($args) {
		echo ReflectionAttribute::getCurrent()->getReflectionTarget();
	}
}

$closure = static function (#[Demo("closure param")] mixed $param) {};

$case = new ReflectionParameter($closure, 'param');
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECT--
Parameter #0 [ <required> mixed $param ]
Parameter #0 [ <required> mixed $param ]
