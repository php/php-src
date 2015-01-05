--TEST--
ReflectionObject::getConstants() - basic function test 
--FILE--
<?php
class C {
	const a = 'hello from C';
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

$classes = array("C", "D", "E", "F", "X");
foreach($classes as $class) {
	echo "Reflecting on instance of class $class: \n";
	$rc = new ReflectionObject(new $class);
	var_dump($rc->getConstants());
}

?>
--EXPECTF--
Reflecting on instance of class C: 
array(1) {
  ["a"]=>
  string(12) "hello from C"
}
Reflecting on instance of class D: 
array(1) {
  ["a"]=>
  string(12) "hello from C"
}
Reflecting on instance of class E: 
array(1) {
  ["a"]=>
  string(12) "hello from C"
}
Reflecting on instance of class F: 
array(1) {
  ["a"]=>
  string(12) "hello from F"
}
Reflecting on instance of class X: 
array(0) {
}
