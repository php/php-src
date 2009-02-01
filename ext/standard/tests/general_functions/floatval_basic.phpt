--TEST--
Testing floatval() and its alias doubleval() Functions
--INI--
precision = 14
--FILE--
<?php
/* Prototype: float floatval( mixed $var );
 * Description: Returns the float value of var.
 */

// different valid  float values 
$valid_floats = array(
       "0.0"  => 0.0,
       "1.0"  => 1.0,
       "-1.0" => -1.0,
       "1.234" => 1.234,
 	   "-1.234" => -1.234,
       "1.2e3" => 1.2e3,
	   "-1.2e3" => -1.2e3,
	   "10.0000000000000000005" => 10.0000000000000000005,
	   "10.5e+5" => 10.5e+5,
       "1e5" => 1e5,
	   "-1e5" => -1e5,
       "1e5" => 1e-5,
	   "-1e-1" => -1e-1,
	   "1e+5" => 1e+5,
	   "-1e+5" =>-1e+5,
	   "1E5" => 1E5,
	   "-1E5" => -1E5,
	   "1E+5" => 1E+5,
	   "-1E5" => -1E+5,
	   ".5e+7" => .5e+7,
	   "-.5e+7" =>-.5e+7
);

/* loop to check that floatval() recognizes different 
   float values, expected output:float value for valid floating point number */
echo "*** Testing floatval() with valid float values ***\n";
foreach ($valid_floats as $key => $value ) {
   echo "\n-- Iteration : $key -- \n";
   var_dump( floatval($value) );
}

/* loop to check that doubleval() also recognizes different 
   float values, expected output:float value for valid floating point number */
echo "\n*** Testing doubleval() with valid float values ***\n";
foreach ($valid_floats as $key => $value ) {
   echo "\n-- Iteration : $key -- \n";
   var_dump( doubleval($value) );
}

?>
===DONE===
--EXPECT--
*** Testing floatval() with valid float values ***

-- Iteration : 0.0 -- 
float(0)

-- Iteration : 1.0 -- 
float(1)

-- Iteration : -1.0 -- 
float(-1)

-- Iteration : 1.234 -- 
float(1.234)

-- Iteration : -1.234 -- 
float(-1.234)

-- Iteration : 1.2e3 -- 
float(1200)

-- Iteration : -1.2e3 -- 
float(-1200)

-- Iteration : 10.0000000000000000005 -- 
float(10)

-- Iteration : 10.5e+5 -- 
float(1050000)

-- Iteration : 1e5 -- 
float(1.0E-5)

-- Iteration : -1e5 -- 
float(-100000)

-- Iteration : -1e-1 -- 
float(-0.1)

-- Iteration : 1e+5 -- 
float(100000)

-- Iteration : -1e+5 -- 
float(-100000)

-- Iteration : 1E5 -- 
float(100000)

-- Iteration : -1E5 -- 
float(-100000)

-- Iteration : 1E+5 -- 
float(100000)

-- Iteration : .5e+7 -- 
float(5000000)

-- Iteration : -.5e+7 -- 
float(-5000000)

*** Testing doubleval() with valid float values ***

-- Iteration : 0.0 -- 
float(0)

-- Iteration : 1.0 -- 
float(1)

-- Iteration : -1.0 -- 
float(-1)

-- Iteration : 1.234 -- 
float(1.234)

-- Iteration : -1.234 -- 
float(-1.234)

-- Iteration : 1.2e3 -- 
float(1200)

-- Iteration : -1.2e3 -- 
float(-1200)

-- Iteration : 10.0000000000000000005 -- 
float(10)

-- Iteration : 10.5e+5 -- 
float(1050000)

-- Iteration : 1e5 -- 
float(1.0E-5)

-- Iteration : -1e5 -- 
float(-100000)

-- Iteration : -1e-1 -- 
float(-0.1)

-- Iteration : 1e+5 -- 
float(100000)

-- Iteration : -1e+5 -- 
float(-100000)

-- Iteration : 1E5 -- 
float(100000)

-- Iteration : -1E5 -- 
float(-100000)

-- Iteration : 1E+5 -- 
float(100000)

-- Iteration : .5e+7 -- 
float(5000000)

-- Iteration : -.5e+7 -- 
float(-5000000)
===DONE===