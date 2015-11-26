--TEST--
ReflectionClass::IsInstantiable()
--FILE--
<?php
class noCtor {
}

class publicCtorNew {
	public function __construct() {}
}

class protectedCtorNew {
	protected function __construct() {}
}

class privateCtorNew {
	private function __construct() {}
}

class publicCtorOld {
	public function publicCtorOld() {}
}

class protectedCtorOld {
	protected function protectedCtorOld() {}
}

class privateCtorOld {
	private function privateCtorOld() {}
}


$classes = array("noCtor", "publicCtorNew", "protectedCtorNew", "privateCtorNew",
				 	"publicCtorOld", "protectedCtorOld", "privateCtorOld");

foreach($classes  as $class ) {
	$reflectionClass = new ReflectionClass($class);
	echo "Is $class instantiable?  ";
	var_dump($reflectionClass->IsInstantiable()); 
}

?>
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; publicCtorOld has a deprecated constructor in %s on line %d

Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; protectedCtorOld has a deprecated constructor in %s on line %d

Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; privateCtorOld has a deprecated constructor in %s on line %d
Is noCtor instantiable?  bool(true)
Is publicCtorNew instantiable?  bool(true)
Is protectedCtorNew instantiable?  bool(false)
Is privateCtorNew instantiable?  bool(false)
Is publicCtorOld instantiable?  bool(true)
Is protectedCtorOld instantiable?  bool(false)
Is privateCtorOld instantiable?  bool(false)
