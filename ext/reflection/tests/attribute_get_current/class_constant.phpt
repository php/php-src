--TEST--
ReflectionAttribute::getCurrent() for class constant
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct( $args ) {
		echo ReflectionAttribute::getCurrent();
	}
}

class WithDemo {

	#[Demo("class constant")]
	public const EXAMPLE = true;

}

$case = new ReflectionClassConstant( WithDemo::class, 'EXAMPLE' );
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECT--
Constant [ public bool EXAMPLE ] { 1 }

Constant [ public bool EXAMPLE ] { 1 }
