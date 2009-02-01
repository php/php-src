--TEST--
Test set_magic_quotes_runtime() function -  usage variation
--INI-
magic_quotes_runtime = 0
--FILE--
<?php
/* Prototype: bool set_magic_quotes_runtime  ( int $new_setting  )
 * Description: Sets the current active configuration setting of magic_quotes_runtime
*/

echo "Simple testcase for set_magic_quotes_runtime() function\n";

$g = get_magic_quotes_runtime();
echo "magic quotes runtime set in INI file: ".$g."\n";

// Prevent notices about undefines variables
error_reporting(E_ALL & ~E_NOTICE);

$unset_var = 10;
unset ($unset_var);

class fooClass {
   function __toString() {
       return "true";
   }
}

$fp = fopen(__FILE__, "r");

$values = array(

	      // int data
/*1*/     0,
	      1,
	      12345,
	      -2345,
	
	      // float data
/*5*/     0.0,
	      10.5,
	      -10.5,
	      10.1234567e10,
	      10.7654321E-10,
	      .5,
	
	      // null data
/*11*/    NULL,
	      null,
	
	      // boolean data
/*13*/    true,
	      false,
	      TRUE,
	      FALSE,
	
	      // empty data
/*17*/    "",
	      '',
	
	      // object data
/*19*/    new fooClass(),
	      
	      // resource
/*20*/    $fp,
	      
	      // undefined data
/*21*/    $undefined_var,
	
	      // unset data
/*22*/    $unset_var
);

// loop through each element of the array for data

$iterator = 1;
foreach($values as $value) {
      echo "-- Iterator $iterator --\n";
      var_dump( set_magic_quotes_runtime($value) );
      echo "New value of magic_quotes_runtime after last set is " . get_magic_quotes_runtime(). "\n";
      $iterator++;
};

fclose($fp); 

?>
===DONE===
--EXPECT--
Simple testcase for set_magic_quotes_runtime() function
magic quotes runtime set in INI file: 0
-- Iterator 1 --
bool(true)
New value of magic_quotes_runtime after last set is 0
-- Iterator 2 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 3 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 4 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 5 --
bool(true)
New value of magic_quotes_runtime after last set is 0
-- Iterator 6 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 7 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 8 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 9 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 10 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 11 --
bool(true)
New value of magic_quotes_runtime after last set is 0
-- Iterator 12 --
bool(true)
New value of magic_quotes_runtime after last set is 0
-- Iterator 13 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 14 --
bool(true)
New value of magic_quotes_runtime after last set is 0
-- Iterator 15 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 16 --
bool(true)
New value of magic_quotes_runtime after last set is 0
-- Iterator 17 --
bool(true)
New value of magic_quotes_runtime after last set is 0
-- Iterator 18 --
bool(true)
New value of magic_quotes_runtime after last set is 0
-- Iterator 19 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 20 --
bool(true)
New value of magic_quotes_runtime after last set is 1
-- Iterator 21 --
bool(true)
New value of magic_quotes_runtime after last set is 0
-- Iterator 22 --
bool(true)
New value of magic_quotes_runtime after last set is 0
===DONE===