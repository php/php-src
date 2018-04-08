--TEST--
Testing floatval() and its alias doubleval() functions : usage variations - different data types as $y arg
--FILE--
<?php
/* Prototype: float floatval( mixed $var );
 * Description: Returns the float value of var.
 */



// get a resource type variable
$fp = fopen (__FILE__, "r");
fclose($fp);
$dfp = opendir ( dirname(__FILE__) );
closedir($dfp);

// other types in an array 
$not_float_types = array (
           "-2147483648" => -2147483648, // max negative integer value
           "2147483647" => 2147483648,  // max positive integer value
           "file resoruce" => $fp, 
           "directory resource" => $dfp,
           "\"0.0\"" => "0.0", // string
           "\"1.0\"" => "1.0",
	       "\"-1.3e3\"" => "-1.3e3",
		   "\"bob-1.3e3\"" => "bob-1.3e3",
           "\"10 Some dollars\"" => "10 Some dollars",
	       "\"10.2 Some Dollars\"" => "10.2 Some Dollars",
	       "\"10.0 dollar\" + 1" => "10.0 dollar" + 1,
		   "\"10.0 dollar\" + 1.0" => "10.0 dollar" + 1.0,
           "\"\"" => "",
           "true" => true,
           "NULL" => NULL,
           "null" => null,
                 );
/* loop through the $not_float_types to see working of 
   floatval() on non float types, expected output: float value valid floating point numbers */
echo "\n*** Testing floatval() on non floating types ***\n";
foreach ($not_float_types as $key => $type ) {
   echo "\n-- Iteration : $key --\n";
   var_dump( floatval($type) );
}

echo "\n*** Testing doubleval() on non floating types ***\n";

/* loop through the $not_float_types to see working of 
   doubleval() on non float types, expected output: float value valid floating point numbers */
foreach ($not_float_types as $key => $type ) {
   echo "\n-- Iteration : $key --\n";
   var_dump( doubleval($type) );
}
?>
===DONE===
--EXPECTF--

Notice: A non well formed numeric value encountered in %s on line %d

Notice: A non well formed numeric value encountered in %s on line %d

*** Testing floatval() on non floating types ***

-- Iteration : -2147483648 --
float(-2147483648)

-- Iteration : 2147483647 --
float(2147483648)

-- Iteration : file resoruce --
float(%d)

-- Iteration : directory resource --
float(%d)

-- Iteration : "0.0" --
float(0)

-- Iteration : "1.0" --
float(1)

-- Iteration : "-1.3e3" --
float(-1300)

-- Iteration : "bob-1.3e3" --
float(0)

-- Iteration : "10 Some dollars" --
float(10)

-- Iteration : "10.2 Some Dollars" --
float(10.2)

-- Iteration : "10.0 dollar" + 1 --
float(11)

-- Iteration : "10.0 dollar" + 1.0 --
float(11)

-- Iteration : "" --
float(0)

-- Iteration : true --
float(1)

-- Iteration : NULL --
float(0)

-- Iteration : null --
float(0)

*** Testing doubleval() on non floating types ***

-- Iteration : -2147483648 --
float(-2147483648)

-- Iteration : 2147483647 --
float(2147483648)

-- Iteration : file resoruce --
float(%d)

-- Iteration : directory resource --
float(%d)

-- Iteration : "0.0" --
float(0)

-- Iteration : "1.0" --
float(1)

-- Iteration : "-1.3e3" --
float(-1300)

-- Iteration : "bob-1.3e3" --
float(0)

-- Iteration : "10 Some dollars" --
float(10)

-- Iteration : "10.2 Some Dollars" --
float(10.2)

-- Iteration : "10.0 dollar" + 1 --
float(11)

-- Iteration : "10.0 dollar" + 1.0 --
float(11)

-- Iteration : "" --
float(0)

-- Iteration : true --
float(1)

-- Iteration : NULL --
float(0)

-- Iteration : null --
float(0)
===DONE===
