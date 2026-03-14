--TEST--
ReflectionAttribute::getCurrent() for global constant
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct( $args ) {
		echo ReflectionAttribute::getCurrent();
	}
}

#[Demo("global constant")]
const GLOBAL_CONSTANT = true;

$case = new ReflectionConstant( 'GLOBAL_CONSTANT' );
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECT--
Constant [ bool GLOBAL_CONSTANT ] { 1 }

Constant [ bool GLOBAL_CONSTANT ] { 1 }
