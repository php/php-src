--TEST--
ReflectionClass::getProperty()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class pubf {
	public $a;
	static public $s;
}
class subpubf extends pubf {
}

class protf {
	protected $a;
	static protected $s;
}
class subprotf extends protf {
}

class privf {
	private $a;
	static protected $s;
}
class subprivf extends privf  {
}

$classes = array("pubf", "subpubf", "protf", "subprotf",
				 "privf", "subprivf");
foreach($classes as $class) {
	echo "Reflecting on class $class: \n";
	$rc = new ReflectionClass($class);
	try {
		echo "  --> Check for s: ";
		var_dump($rc->getProperty("s"));
	} catch (exception $e) {
		echo $e->getMessage() . "\n";
	}
	try {
		echo "  --> Check for a: ";
		var_dump($rc->getProperty("a"));
	} catch (exception $e) {
		echo $e->getMessage() . "\n";
	}
	try {
		echo "  --> Check for A: ";
		var_dump($rc->getProperty("A"));
	} catch (exception $e) {
		echo $e->getMessage() . "\n";
	}
	try {
		echo "  --> Check for doesntExist: ";
		var_dump($rc->getProperty("doesntExist"));
	} catch (exception $e) {
		echo $e->getMessage() . "\n";
	}

}
?>
--EXPECTF--
Reflecting on class pubf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(4) "pubf"
}
  --> Check for a: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(4) "pubf"
}
  --> Check for A: Property A does not exist
  --> Check for doesntExist: Property doesntExist does not exist
Reflecting on class subpubf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(4) "pubf"
}
  --> Check for a: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(4) "pubf"
}
  --> Check for A: Property A does not exist
  --> Check for doesntExist: Property doesntExist does not exist
Reflecting on class protf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(5) "protf"
}
  --> Check for a: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(5) "protf"
}
  --> Check for A: Property A does not exist
  --> Check for doesntExist: Property doesntExist does not exist
Reflecting on class subprotf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(5) "protf"
}
  --> Check for a: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(5) "protf"
}
  --> Check for A: Property A does not exist
  --> Check for doesntExist: Property doesntExist does not exist
Reflecting on class privf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(5) "privf"
}
  --> Check for a: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(5) "privf"
}
  --> Check for A: Property A does not exist
  --> Check for doesntExist: Property doesntExist does not exist
Reflecting on class subprivf: 
  --> Check for s: object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(1) "s"
  ["class"]=>
  string(5) "privf"
}
  --> Check for a: Property a does not exist
  --> Check for A: Property A does not exist
  --> Check for doesntExist: Property doesntExist does not exist
