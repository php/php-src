--TEST--
ReflectionObject::getConstructor() - basic function test
--FILE--
<?php
class NewCtor {
	function __construct() {}
}

class ExtendsNewCtor extends NewCtor {
}

$classes = array('NewCtor', 'ExtendsNewCtor'); 

foreach ($classes as $class) {
	$rc = new ReflectionObject(new $class);
	$rm = $rc->getConstructor();
	if ($rm != null) {
		echo "Constructor of $class: " . $rm->getName() . "\n";
	}  else {
		echo "No constructor for $class\n";
	}
	
}				 
				
?>
--EXPECTF--
Constructor of NewCtor: __construct
Constructor of ExtendsNewCtor: __construct
