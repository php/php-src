--TEST--
Test var_export() function with valid float values
--INI--
serialize_precision=17
--FILE--
<?php
/* Prototype  : mixed var_export(mixed var [, bool return])
 * Description: Outputs or returns a string representation of a variable 
 * Source code: ext/standard/var.c
 * Alias to functions: 
 */

echo "*** Testing var_export() with valid float values ***\n";
// different valid  float vlaues 
$valid_floats = array(
	  "-2147483649" => -2147483649, // float value
	  "2147483648" => 2147483648,  // float value
	  "-0x80000001" => -0x80000001, // float value, beyond max negative int
	  "0x800000001" => 0x800000001, // float value, beyond max positive int
	  "020000000001" => 020000000001, // float value, beyond max positive int
	  "-020000000001" => -020000000001, // float value, beyond max negative int
	  "0.0" => 0.0,
	  "-0.1" => -0.1,
	  "10.0000000000000000005" => 10.0000000000000000005,
	  "10.5e+5" => 10.5e+5,
	  "1e5" => 1e5,
	  "1e-5" => 1e-5,
	  "1e+5" => 1e+5,
	  "1E5" => 1E5,
	  "1E+5" => 1E+5,
	  "1E-5" => 1E-5,
	  ".5e+7" => .5e+7,
	  ".6e-19" => .6e-19,
	  ".05E+44" => .05E+44,
	  ".0034E-30" => .0034E-30
);
/* Loop to check for above float values with var_export() */
echo "\n*** Output for float values ***\n";
foreach($valid_floats as $key => $float_value) {
	echo "\n-- Iteration: $key --\n";
	var_export( $float_value );
	echo "\n";
	var_export( $float_value, FALSE);
	echo "\n";
	var_dump( var_export( $float_value, TRUE) );
	echo "\n";
}

?>
===DONE===
--EXPECT--
*** Testing var_export() with valid float values ***

*** Output for float values ***

-- Iteration: -2147483649 --
-2147483649
-2147483649
string(11) "-2147483649"


-- Iteration: 2147483648 --
2147483648
2147483648
string(10) "2147483648"


-- Iteration: -0x80000001 --
-2147483649
-2147483649
string(11) "-2147483649"


-- Iteration: 0x800000001 --
34359738369
34359738369
string(11) "34359738369"


-- Iteration: 020000000001 --
2147483649
2147483649
string(10) "2147483649"


-- Iteration: -020000000001 --
-2147483649
-2147483649
string(11) "-2147483649"


-- Iteration: 0.0 --
0
0
string(1) "0"


-- Iteration: -0.1 --
-0.10000000000000001
-0.10000000000000001
string(20) "-0.10000000000000001"


-- Iteration: 10.0000000000000000005 --
10
10
string(2) "10"


-- Iteration: 10.5e+5 --
1050000
1050000
string(7) "1050000"


-- Iteration: 1e5 --
100000
100000
string(6) "100000"


-- Iteration: 1e-5 --
1.0000000000000001E-5
1.0000000000000001E-5
string(21) "1.0000000000000001E-5"


-- Iteration: 1e+5 --
100000
100000
string(6) "100000"


-- Iteration: 1E5 --
100000
100000
string(6) "100000"


-- Iteration: 1E+5 --
100000
100000
string(6) "100000"


-- Iteration: 1E-5 --
1.0000000000000001E-5
1.0000000000000001E-5
string(21) "1.0000000000000001E-5"


-- Iteration: .5e+7 --
5000000
5000000
string(7) "5000000"


-- Iteration: .6e-19 --
6.0000000000000006E-20
6.0000000000000006E-20
string(22) "6.0000000000000006E-20"


-- Iteration: .05E+44 --
5.0000000000000001E+42
5.0000000000000001E+42
string(22) "5.0000000000000001E+42"


-- Iteration: .0034E-30 --
3.4000000000000001E-33
3.4000000000000001E-33
string(22) "3.4000000000000001E-33"

===DONE===
