--TEST--
Test println() function : basic functionality
--FILE--
<?php

/* Prototype  : void println ( string $arg )
 * Description: Output a string
 * Source code: ext/standard/basic_functions.c
*/

echo "*** Testing println() : basic functionality ***\n";

echo "-- Iteration 1 --\n";
println("Hello World");

echo "-- Iteration 2 --\n";
println("This spans
multiple lines. The newlines will be
output as well");

echo "-- Iteration 3 --\n";
println("escaping characters is done \"Like this\".");

// You can use variables inside of a println statement
$foo = "foobar";
$bar = "barbaz";

echo "-- Iteration 4 --\n";
println("foo is $foo"); // foo is foobar

// You can also use arrays
$bar = array("value" => "foo");

echo "-- Iteration 5 --\n";
println("this is {$bar['value']} !"); // this is foo !

// Using single quotes will output the variable name, not the value
echo "-- Iteration 6 --\n";
println('foo is $foo'); // foo is $foo

// If you are not using any other characters, you can just output variables
echo "-- Iteration 7 --\n";
println($foo); // foobar

echo "-- Iteration 8 --\n";
$variable = "VARIABLE";
println(<<<END
This uses the "here document" syntax to output
multiple lines with $variable interpolation. Note
that the here document terminator must appear on a
line with just a semicolon no extra whitespace!
END
);
?>
===DONE===
--EXPECT--
*** Testing println() : basic functionality ***
-- Iteration 1 --
Hello World
-- Iteration 2 --
This spans
multiple lines. The newlines will be
output as well
-- Iteration 3 --
escaping characters is done "Like this".
-- Iteration 4 --
foo is foobar
-- Iteration 5 --
this is foo !
-- Iteration 6 --
foo is $foo
-- Iteration 7 --
foobar
-- Iteration 8 --
This uses the "here document" syntax to output
multiple lines with VARIABLE interpolation. Note
that the here document terminator must appear on a
line with just a semicolon no extra whitespace!
===DONE===
