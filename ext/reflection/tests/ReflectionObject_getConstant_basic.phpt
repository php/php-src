--TEST--
ReflectionObject::getConstant() basic function test
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
    var_dump($rc->getConstant('a'));
    var_dump($rc->getConstant('doesNotexist'));
}
?>
--EXPECT--
Reflecting on instance of class C: 
string(12) "hello from C"
bool(false)
Reflecting on instance of class D: 
string(12) "hello from C"
bool(false)
Reflecting on instance of class E: 
string(12) "hello from C"
bool(false)
Reflecting on instance of class F: 
string(12) "hello from F"
bool(false)
Reflecting on instance of class X: 
bool(false)
bool(false)
