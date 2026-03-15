--TEST--
ReflectionAttribute::getCurrent() for function
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct( $args ) {
		echo ReflectionAttribute::getCurrent();
	}
}

#[Demo("function")]
function globalFunc(
	mixed $param
) {}

$case = new ReflectionFunction( 'globalFunc' );
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECTF--
Function [ <user> function globalFunc ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}

Function [ <user> function globalFunc ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}
