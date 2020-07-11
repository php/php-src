--TEST--
Check behaviour of incomplete class
--FILE--
<?php
$serialized = 'O:1:"C":1:{s:1:"p";i:1;}';

$incomplete = unserialize($serialized);
eval('Class C {}');
$complete   = unserialize($serialized);


echo "\n\n---> Various types of access on complete class:\n" ;
var_dump($complete);
var_dump(is_object($complete));
var_dump($complete->p);

$ref1 = "ref1.original";
$complete->p = &$ref1;
var_dump($complete->p);
$ref1 = "ref1.changed";
var_dump($complete->p);
$complete->p = "p.changed";
var_dump($ref1);

var_dump(isset($complete->x));
$complete->x = "x.new";
var_dump(isset($complete->x));
unset($complete->x);
var_dump($complete->x);


echo "\n\n---> Same types of access on incomplete class:\n" ;
var_dump($incomplete);
var_dump(is_object($incomplete));
var_dump($incomplete->p);

$ref2 = "ref1.original";
$incomplete->p = &$ref2;
var_dump($incomplete->p);
$ref2 = "ref1.changed";
var_dump($incomplete->p);
$incomplete->p = "p.changed";
var_dump($ref1);

var_dump(isset($incomplete->x));
$incomplete->x = "x.new";
var_dump(isset($incomplete->x));
unset($incomplete->x);
var_dump($incomplete->x);

$incomplete->f();

echo "Done";
?>
--EXPECTF--
---> Various types of access on complete class:
object(C)#%d (1) {
  ["p"]=>
  int(1)
}
bool(true)
int(1)
string(13) "ref1.original"
string(12) "ref1.changed"
string(9) "p.changed"
bool(false)
bool(true)

Warning: Undefined property: C::$x in %s on line %d
NULL


---> Same types of access on incomplete class:
object(__PHP_Incomplete_Class)#%d (2) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(1) "C"
  ["p"]=>
  int(1)
}
bool(true)

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "C" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
NULL

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "C" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "C" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
NULL

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "C" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
NULL

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "C" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
string(9) "p.changed"

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "C" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
bool(false)

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "C" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "C" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
bool(false)

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "C" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d

Notice: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "C" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
NULL

Fatal error: main(): The script tried to execute a method or access a property of an incomplete object. Please ensure that the class definition "C" of the object you are trying to operate on was loaded _before_ unserialize() gets called or provide an autoloader to load the class definition in %s on line %d
