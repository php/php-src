--TEST--
ReflectionClass::getProperties()
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
	static private $s;
}
class subprivf extends privf  {
}

$classes = array("pubf", "subpubf", "protf", "subprotf",
				 "privf", "subprivf");
foreach($classes as $class) {
	echo "Reflecting on class $class: \n";
	$rc = new ReflectionClass($class);
	var_dump($rc->getProperties());
}

?>
--EXPECTF--
Reflecting on class pubf: 
array(2) {
  [0]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "a"
    ["class"]=>
    string(4) "pubf"
  }
  [1]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "s"
    ["class"]=>
    string(4) "pubf"
  }
}
Reflecting on class subpubf: 
array(2) {
  [0]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "a"
    ["class"]=>
    string(4) "pubf"
  }
  [1]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "s"
    ["class"]=>
    string(4) "pubf"
  }
}
Reflecting on class protf: 
array(2) {
  [0]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "a"
    ["class"]=>
    string(5) "protf"
  }
  [1]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "s"
    ["class"]=>
    string(5) "protf"
  }
}
Reflecting on class subprotf: 
array(2) {
  [0]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "a"
    ["class"]=>
    string(5) "protf"
  }
  [1]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "s"
    ["class"]=>
    string(5) "protf"
  }
}
Reflecting on class privf: 
array(2) {
  [0]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "a"
    ["class"]=>
    string(5) "privf"
  }
  [1]=>
  object(ReflectionProperty)#%d (2) {
    ["name"]=>
    string(1) "s"
    ["class"]=>
    string(5) "privf"
  }
}
Reflecting on class subprivf: 
array(0) {
}
