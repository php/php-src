--TEST--
ReflectionAttribute::getCurrent() for method
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct( $args ) {
		echo ReflectionAttribute::getCurrent();
	}
}

class WithDemo {

	#[Demo("method")]
	public function method(
		mixed $param
	) {}
}

$case = new ReflectionMethod( WithDemo::class, 'method' );
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECTF--
Method [ <user> public method method ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}

Method [ <user> public method method ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}
