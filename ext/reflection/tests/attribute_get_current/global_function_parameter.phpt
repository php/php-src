--TEST--
ReflectionAttribute::getCurrent() for function parameter
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct( $args ) {
		echo ReflectionAttribute::getCurrent();
	}
}

function globalFunc(
	#[Demo("function param")] mixed $param
) {}

$case = new ReflectionParameter( 'globalFunc', 'param' );
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECT--
Parameter #0 [ <required> mixed $param ]
Parameter #0 [ <required> mixed $param ]
