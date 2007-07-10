--TEST--
ReflectionClass::getConstructor()
--FILE--
<?php
class NewCtor {
	function __construct() {}
}

class ExtendsNewCtor extends NewCtor {
}

class OldCtor {
	function OldCtor() {}
}

class ExtendsOldCtor extends OldCtor {
}


class X {
	function Y() {}
}

class Y extends X {
}

class OldAndNewCtor {
	function OldAndNewCtor() {}
	function __construct() {}
}

class NewAndOldCtor {
	function __construct() {}
	function NewAndOldCtor() {}
}
class B {
	function B() {}
}

class C extends B {
	function C() {}
}

class D1 extends C {
	function __construct() {}
}

class D2 extends C {
}

$classes = array('NewCtor', 'ExtendsNewCtor', 'OldCtor', 'ExtendsOldCtor', 
				 'OldAndNewCtor', 'NewAndOldCtor', 'B', 'C', 'D1', 'D2', 'X', 'Y');

foreach ($classes as $class) {
	$rc = new ReflectionClass($class);
	$rm = $rc->getConstructor();
	if ($rm != null) {
		echo "Constructor of $class: " . $rm->getName() . "\n";
	}  else {
		echo "No constructor for $class\n";
	}
	
}				 
				 
?>
--EXPECTF--

Strict Standards: Redefining already defined constructor for class OldAndNewCtor in %s on line 26

Strict Standards: %s for class NewAndOldCtor in %s on line 31
Constructor of NewCtor: __construct
Constructor of ExtendsNewCtor: __construct
Constructor of OldCtor: OldCtor
Constructor of ExtendsOldCtor: OldCtor
Constructor of OldAndNewCtor: __construct
Constructor of NewAndOldCtor: __construct
Constructor of B: B
Constructor of C: C
Constructor of D1: __construct
Constructor of D2: C
No constructor for X
No constructor for Y