--TEST--
Test sha1() function : usage variations - unexpected values for 'str' argument
--FILE--
<?php

/* Prototype: string sha1  ( string $str  [, bool $raw_output  ] )
 * Description: Calculate the sha1 hash of a string
 */

echo "*** Testing sha1() : unexpected values for 'str' ***\n";

$raw = false;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//defining class for object variable
class MyClass
{
  public function __toString()
  {
    return "object";
  }
}

//resource variable
$fp = fopen(__FILE__, 'r');

//different values for 'str' argument
$values = array(

		  // int data
/*1*/	  0,
		  1,
		  12345,
		  -2345,

		  // float data
/*5*/	  10.5,
		  -10.5,
		  10.1234567e10,
		  10.1234567E-10,
		  .5,

		  // array data
/*10*/	  array(),
		  array(0),
		  array(1),
		  array(1, 2),
		  array('color' => 'red', 'item' => 'pen'),

		  // null data
/*15*/	  NULL,
		  null,

		  // boolean data
/*17*/	  true,
		  false,
		  TRUE,
		  FALSE,

		  // empty data
/*21*/	  "",
		  '',

		  // object data
/*23*/	  new MyClass(),

		  // undefined data
/*24*/	  @$undefined_var,

		  // unset data
/*25*/	  @$unset_var,

		  //resource data
/*26*/	  $fp
);

// loop through each element of $values for 'str' argument
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count+1)." --\n";
  var_dump( sha1($values[$count], $raw) );
}

//closing resource
fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing sha1() : unexpected values for 'str' ***
-- Iteration 1 --
string(40) "b6589fc6ab0dc82cf12099d1c2d40ab994e8410c"
-- Iteration 2 --
string(40) "356a192b7913b04c54574d18c28d46e6395428ab"
-- Iteration 3 --
string(40) "8cb2237d0679ca88db6464eac60da96345513964"
-- Iteration 4 --
string(40) "bc97c643aba3b6c6abe253222f439d4002a87528"
-- Iteration 5 --
string(40) "1287384bc5ef3ab84a36a5ef1d888df2763567f4"
-- Iteration 6 --
string(40) "c9d6e1b691f17c8ae6d458984a5f56f80e62a60b"
-- Iteration 7 --
string(40) "39493e1e645578a655f532e1f9bcff67991f2c2f"
-- Iteration 8 --
string(40) "681b45cae882ad795afd54ccc2a04ad58e056b83"
-- Iteration 9 --
string(40) "1b390cd54a0c0d4f27fa7adf23e3c45536e9f37c"
-- Iteration 10 --

Warning: sha1() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: sha1() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: sha1() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: sha1() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: sha1() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 15 --
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
-- Iteration 16 --
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
-- Iteration 17 --
string(40) "356a192b7913b04c54574d18c28d46e6395428ab"
-- Iteration 18 --
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
-- Iteration 19 --
string(40) "356a192b7913b04c54574d18c28d46e6395428ab"
-- Iteration 20 --
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
-- Iteration 21 --
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
-- Iteration 22 --
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
-- Iteration 23 --
string(40) "1615307cc4523f183e777df67f168c86908e8007"
-- Iteration 24 --
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
-- Iteration 25 --
string(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
-- Iteration 26 --

Warning: sha1() expects parameter 1 to be string, resource given in %s on line %d
NULL
===DONE===
