--TEST--
Testing floatval() different data types as arg
--INI--
precision=14
--FILE--
<?php

// other types in an array
$not_float_types = [
	"-2147483648" => -2147483648, // max negative integer value
	"2147483647" => 2147483648,  // max positive integer value
	"stream resource" => STDERR,
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
	"false" => false,
	"null" => null,
];
/* loop through the $not_float_types to see working of
   floatval() on non float types, expected output: float value valid floating point numbers */
echo "\n*** Testing floatval() on non floating types ***\n";
foreach ($not_float_types as $key => $type ) {
   echo "\n-- Iteration : $key --\n";
   var_dump( floatval($type) );
}

?>
--EXPECTF--
Warning: A non-numeric value encountered in %s on line %d

Warning: A non-numeric value encountered in %s on line %d

*** Testing floatval() on non floating types ***

-- Iteration : -2147483648 --
float(-2147483648)

-- Iteration : 2147483647 --
float(2147483648)

-- Iteration : stream resource --
float(3)

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

-- Iteration : false --
float(0)

-- Iteration : null --
float(0)
