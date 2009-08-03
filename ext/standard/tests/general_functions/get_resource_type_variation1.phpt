--TEST--
Test get_resource_type() function : usage variations - different data types as handle arg
--FILE--
<?php
/* Prototype  : string get_resource_type  ( resource $handle  )
 * Description:  Returns the resource type 
 * Source code: Zend/zend_builtin_functions.c
 */		

echo "*** Testing get_resource_type() : variation test ***\n";

class Hello {
  public function SayHello($arg) {
  	echo "Hello\n";
  } 
}

$res = fopen(__FILE__, "r");

$vars = array(
	"bool"=>true,
	"int 10"=>10,
	"float 10.5"=>10.5,
	"string"=>"Hello World",
	"array"=>array(1,2,3,4,5),
	"NULL"=>NULL,
	"Object"=>new Hello()
);

foreach($vars as $variation =>$object) {
      echo "\n-- $variation --\n";
      var_dump(get_resource_type($object));
};

?>
===DONE===
--EXPECTF--
*** Testing get_resource_type() : variation test ***

-- bool --

Warning: get_resource_type() expects parameter 1 to be resource, boolean given in %s on line %d
NULL

-- int 10 --

Warning: get_resource_type() expects parameter 1 to be resource, integer given in %s on line %d
NULL

-- float 10.5 --

Warning: get_resource_type() expects parameter 1 to be resource, double given in %s on line %d
NULL

-- string --

Warning: get_resource_type() expects parameter 1 to be resource, string given in %s on line %d
NULL

-- array --

Warning: get_resource_type() expects parameter 1 to be resource, array given in %s on line %d
NULL

-- NULL --

Warning: get_resource_type() expects parameter 1 to be resource, null given in %s on line %d
NULL

-- Object --

Warning: get_resource_type() expects parameter 1 to be resource, object given in %s on line %d
NULL
===DONE===