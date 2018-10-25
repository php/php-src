--TEST--
Test php_uname() function -  usage variations
--FILE--
<?php
/* Prototype: string php_uname  ([ string $mode  ] )
 * Description:  Returns information about the operating system PHP is running on
*/

echo "*** Testing php_uname() - usage variations\n";
// Prevent notices about undefines variables
error_reporting(E_ALL & ~E_NOTICE);

$unset_var = 10;
unset ($unset_var);

class fooClass {
   function __toString() {
       return "m";
   }
}

$values = array(

		  // int data
		  "0" => 0,
		  "1" => 1,
		  "12345" =>  12345,
		  "-2345" =>  -2345,

		  // float data
		  "10.5" => 10.5,
		  "-10.5" => -10.5,
		  "10.1234567e10" => 10.1234567e10,
		  "10.7654321E-10" => 10.7654321E-10,
		  ".5" => .5,

		  // null data
		  "NULL" => NULL,
		  "null" =>  null,

		  // boolean data
		  "true" => true,
		  "false" => false,
		  "TRUE" => TRUE,
		  "FALSE" => FALSE,

		  // empty data
		  "\"\"" => "",
		  "''" => '',

		  // object data
		  "new fooClass()" => new fooClass(),

		  // undefined data
		  "undefined var" => $undefined_var,

		 // unset data
		 "unset var" => $unset_var,
);

// loop through each element of the array for data

foreach($values as $key => $value) {
      echo "-- Iterator $key --\n";
      var_dump( php_uname($value) );
};

?>
===DONE===
--EXPECTF--
*** Testing php_uname() - usage variations
-- Iterator 0 --
string(%d) "%s"
-- Iterator 1 --
string(%d) "%s"
-- Iterator 12345 --
string(%d) "%s"
-- Iterator -2345 --
string(%d) "%s"
-- Iterator 10.5 --
string(%d) "%s"
-- Iterator -10.5 --
string(%d) "%s"
-- Iterator 10.1234567e10 --
string(%d) "%s"
-- Iterator 10.7654321E-10 --
string(%d) "%s"
-- Iterator .5 --
string(%d) "%s"
-- Iterator NULL --
string(%d) "%s"
-- Iterator null --
string(%d) "%s"
-- Iterator true --
string(%d) "%s"
-- Iterator false --
string(%d) "%s"
-- Iterator TRUE --
string(%d) "%s"
-- Iterator FALSE --
string(%d) "%s"
-- Iterator "" --
string(%d) "%s"
-- Iterator '' --
string(%d) "%s"
-- Iterator new fooClass() --
string(%d) "%s"
-- Iterator undefined var --
string(%d) "%s"
-- Iterator unset var --
string(%d) "%s"
===DONE===
