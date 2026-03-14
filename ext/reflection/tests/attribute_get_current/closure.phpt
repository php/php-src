--TEST--
ReflectionAttribute::getCurrent() for closure
--FILE--
<?php

#[Attribute]
class Demo {
	public function __construct( $args ) {
		echo ReflectionAttribute::getCurrent();
	}
}

$closure = #[Demo("closure")] static function ( mixed $param ) {};

$case = new ReflectionFunction( $closure );
echo $case;
echo "\n";
$case->getAttributes()[0]->newInstance();

?>
--EXPECTF--
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}

Closure [ <user> static function {closure:%s:%d} ] {
  @@ %s %d - %d

  - Parameters [1] {
    Parameter #0 [ <required> mixed $param ]
  }
}
