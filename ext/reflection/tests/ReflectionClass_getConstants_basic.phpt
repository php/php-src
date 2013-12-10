--TEST--
ReflectionClass::getConstants()
--FILE--
<?php
class C {
	const a = 'hello from C';
	const b = self::a . ' - const expr';
}
class D extends C {
}
class E extends D {
}
class F extends E {
	const a = 'hello from F';
}
class X {
}

$classes = array('C', 'D', 'E', 'F', 'X');
foreach($classes as $class) {
	echo "Constants from class $class: \n";
	$rc = new ReflectionClass($class);
	var_dump($rc->getConstants());
}
?>
--EXPECTF--
Constants from class C: 
array(2) {
  ["a"]=>
  string(12) "hello from C"
  ["b"]=>
  string(25) "hello from C - const expr"
}
Constants from class D: 
array(2) {
  ["a"]=>
  string(12) "hello from C"
  ["b"]=>
  string(25) "hello from C - const expr"
}
Constants from class E: 
array(2) {
  ["a"]=>
  string(12) "hello from C"
  ["b"]=>
  string(25) "hello from C - const expr"
}
Constants from class F: 
array(2) {
  ["a"]=>
  string(12) "hello from F"
  ["b"]=>
  string(25) "hello from C - const expr"
}
Constants from class X: 
array(0) {
}
