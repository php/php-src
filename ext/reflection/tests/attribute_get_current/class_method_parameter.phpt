--TEST--
ReflectionAttribute::getCurrent() for method parameter
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct( $args ) {
		echo ReflectionAttribute::getCurrent();
	}
}

class WithDemo {

	public function method(
		#[Demo("method param")] mixed $param
	) {}

}

$case = new ReflectionParameter( [ WithDemo::class, 'method' ], 'param' );
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECT--
Parameter #0 [ <required> mixed $param ]
Parameter #0 [ <required> mixed $param ]
