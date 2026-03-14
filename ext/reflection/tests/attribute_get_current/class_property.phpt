--TEST--
ReflectionAttribute::getCurrent() for property
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct( $args ) {
		echo ReflectionAttribute::getCurrent();
	}
}

class WithDemo {

	#[Demo("property")]
	public mixed $prop;

}

$case = new ReflectionProperty( WithDemo::class, 'prop' );
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECT--
Property [ public mixed $prop ]

Property [ public mixed $prop ]
