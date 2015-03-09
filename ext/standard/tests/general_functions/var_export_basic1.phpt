--TEST--
Test var_export() function with integer values
--FILE--
<?php
/* Prototype  : mixed var_export(mixed var [, bool return])
 * Description: Outputs or returns a string representation of a variable 
 * Source code: ext/standard/var.c
 * Alias to functions: 
 */

echo "*** Testing var_export() with integer values ***\n";
// different integer vlaues 
$valid_ints = array(
                '0' => '0',
                '1' => '1',
                '-1' => '-1',
                '-2147483648' => '-2147483648', // max negative integer value
                '-2147483647' => '-2147483647', 
                '2147483647' => 2147483647,  // max positive integer value
                '2147483640' => 2147483640,
                '0x123B' => 0x123B,      // integer as hexadecimal
                "'0x12ab'" => '0x12ab',
                "'0Xfff'" => '0Xfff',
                "'0XFA'" => '0XFA',
                "-0x80000000" => -0x80000000, // max negative integer as hexadecimal
                "'0x7fffffff'" => '0x7fffffff',  // max positive integer as hexadecimal
                "0x7FFFFFFF" => 0x7FFFFFFF,  // max positive integer as hexadecimal
                "'0123'" => '0123',        // integer as octal
                "01912" => 01,       // should be quivalent to octal 1
                "-020000000000" => -020000000000, // max negative integer as octal
                "017777777777" => 017777777777,  // max positive integer as octal
);

/* Loop to check for above integer values with var_export() */
echo "\n*** Output for integer values ***\n";
foreach($valid_ints as $key => $int_value) {
	echo "\n-- Iteration: $key --\n";
	var_export( $int_value );
	echo "\n";
	var_export( $int_value, FALSE);
	echo "\n";
	var_dump( var_export( $int_value, TRUE) );
}

?>
===DONE===
--EXPECT--
*** Testing var_export() with integer values ***

*** Output for integer values ***

-- Iteration: 0 --
'0'
'0'
string(3) "'0'"

-- Iteration: 1 --
'1'
'1'
string(3) "'1'"

-- Iteration: -1 --
'-1'
'-1'
string(4) "'-1'"

-- Iteration: -2147483648 --
'-2147483648'
'-2147483648'
string(13) "'-2147483648'"

-- Iteration: -2147483647 --
'-2147483647'
'-2147483647'
string(13) "'-2147483647'"

-- Iteration: 2147483647 --
2147483647
2147483647
string(10) "2147483647"

-- Iteration: 2147483640 --
2147483640
2147483640
string(10) "2147483640"

-- Iteration: 0x123B --
4667
4667
string(4) "4667"

-- Iteration: '0x12ab' --
'0x12ab'
'0x12ab'
string(8) "'0x12ab'"

-- Iteration: '0Xfff' --
'0Xfff'
'0Xfff'
string(7) "'0Xfff'"

-- Iteration: '0XFA' --
'0XFA'
'0XFA'
string(6) "'0XFA'"

-- Iteration: -0x80000000 --
-2147483648
-2147483648
string(11) "-2147483648"

-- Iteration: '0x7fffffff' --
'0x7fffffff'
'0x7fffffff'
string(12) "'0x7fffffff'"

-- Iteration: 0x7FFFFFFF --
2147483647
2147483647
string(10) "2147483647"

-- Iteration: '0123' --
'0123'
'0123'
string(6) "'0123'"

-- Iteration: 01912 --
1
1
string(1) "1"

-- Iteration: -020000000000 --
-2147483648
-2147483648
string(11) "-2147483648"

-- Iteration: 017777777777 --
2147483647
2147483647
string(10) "2147483647"
===DONE===
