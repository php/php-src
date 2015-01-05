--TEST--
ReflectionClass::getConstants()
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
	echo "Reflecting on class $class: \n";
	$rc = new ReflectionClass($class);
	var_dump($rc->getConstant('a'));
	var_dump($rc->getConstant('doesntexist'));
}
?>
--EXPECTF--
Reflecting on class C: 
string(12) "hello from C"
bool(false)
Reflecting on class D: 
string(12) "hello from C"
bool(false)
Reflecting on class E: 
string(12) "hello from C"
bool(false)
Reflecting on class F: 
string(12) "hello from F"
bool(false)
Reflecting on class X: 
bool(false)
bool(false)
