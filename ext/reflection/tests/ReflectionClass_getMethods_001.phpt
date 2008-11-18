--TEST--
ReflectionClass::getMethods()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class pubf {
	public function f() {}
	static public function s() {}	
}
class subpubf extends pubf {
}

class protf {
	protected function f() {}
	static protected function s() {}	
}
class subprotf extends protf {
}

class privf {
	private function f() {}
	static private function s() {}
}
class subprivf extends privf  {
}

$classes = array("pubf", "subpubf", "protf", "subprotf", 
				 "privf", "subprivf");
foreach($classes as $class) {
	echo "Reflecting on class $class: \n";
	$rc = new ReflectionClass($class);
	var_dump($rc->getMethods());
}

?>
--EXPECTF--
Reflecting on class pubf: 
array(2) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "f"
    ["class"]=>
    string(4) "pubf"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "s"
    ["class"]=>
    string(4) "pubf"
  }
}
Reflecting on class subpubf: 
array(2) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "f"
    ["class"]=>
    string(7) "subpubf"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "s"
    ["class"]=>
    string(7) "subpubf"
  }
}
Reflecting on class protf: 
array(2) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "f"
    ["class"]=>
    string(5) "protf"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "s"
    ["class"]=>
    string(5) "protf"
  }
}
Reflecting on class subprotf: 
array(2) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "f"
    ["class"]=>
    string(8) "subprotf"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "s"
    ["class"]=>
    string(8) "subprotf"
  }
}
Reflecting on class privf: 
array(2) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "f"
    ["class"]=>
    string(5) "privf"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "s"
    ["class"]=>
    string(5) "privf"
  }
}
Reflecting on class subprivf: 
array(2) {
  [0]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "f"
    ["class"]=>
    string(8) "subprivf"
  }
  [1]=>
  &object(ReflectionMethod)#%d (2) {
    ["name"]=>
    string(1) "s"
    ["class"]=>
    string(8) "subprivf"
  }
}