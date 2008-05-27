--TEST--
ReflectionObject::isInstance() - basic function test
--FILE--
<?php
class A {}
class B extends A {}
class X {}

$classes = array("A", "B", "X");

$instances = array(	"myA" => new A,
					"myB" => new B,
					"myX" => new X );

foreach ($classes as $class) {
	$ro = new ReflectionObject(new $class);
	foreach ($instances as $name => $instance) {
		echo "is $name a $class? ";
		var_dump($ro->isInstance($instance));	
	}
}

?>
--EXPECTF--
is myA a A? bool(true)
is myB a A? bool(true)
is myX a A? bool(false)
is myA a B? bool(false)
is myB a B? bool(true)
is myX a B? bool(false)
is myA a X? bool(false)
is myB a X? bool(false)
is myX a X? bool(true)
