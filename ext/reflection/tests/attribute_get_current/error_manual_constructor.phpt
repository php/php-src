--TEST--
ReflectionAttribute::getCurrent() when attribute constructed manually
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

new Demo(null);

?>
--EXPECTF--
Constant [ bool GLOBAL_CONSTANT ] { 1 }

Constant [ bool GLOBAL_CONSTANT ] { 1 }

Fatal error: Uncaught Error: Current function was not invoked via ReflectionAttribute::newInstance() in %s:%d
Stack trace:
#0 %s(%d): ReflectionAttribute::getCurrent()
#1 %s(%d): Demo->__construct(NULL)
#2 {main}
  thrown in %s on line %d
