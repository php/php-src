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
unicode(%d) "%s"
-- Iterator 1 --
unicode(%d) "%s"
-- Iterator 12345 --
unicode(%d) "%s"
-- Iterator -2345 --
unicode(%d) "%s"
-- Iterator 10.5 --
unicode(%d) "%s"
-- Iterator -10.5 --
unicode(%d) "%s"
-- Iterator 10.1234567e10 --
unicode(%d) "%s"
-- Iterator 10.7654321E-10 --
unicode(%d) "%s"
-- Iterator .5 --
unicode(%d) "%s"
-- Iterator NULL --
unicode(%d) "%s"
-- Iterator null --
unicode(%d) "%s"
-- Iterator true --
unicode(%d) "%s"
-- Iterator false --
unicode(%d) "%s"
-- Iterator TRUE --
unicode(%d) "%s"
-- Iterator FALSE --
unicode(%d) "%s"
-- Iterator "" --
unicode(%d) "%s"
-- Iterator '' --
unicode(%d) "%s"
-- Iterator new fooClass() --
unicode(%d) "%s"
-- Iterator undefined var --
unicode(%d) "%s"
-- Iterator unset var --
unicode(%d) "%s"
===DONE===