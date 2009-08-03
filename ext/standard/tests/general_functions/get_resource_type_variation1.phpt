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
	"bool"=>false,
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

Warning: Supplied argument is not a valid resource handle in %s on line %d
bool(false)

-- int 10 --

Warning: Supplied argument is not a valid resource handle in %s on line %d
bool(false)

-- float 10.5 --

Warning: Supplied argument is not a valid resource handle in %s on line %d
bool(false)

-- string --

Warning: Supplied argument is not a valid resource handle in %s on line %d
bool(false)

-- array --

Warning: Supplied argument is not a valid resource handle in %s on line %d
bool(false)

-- NULL --

Warning: Supplied argument is not a valid resource handle in %s on line %d
bool(false)

-- Object --

Warning: Supplied argument is not a valid resource handle in %s on line %d
bool(false)
===DONE===