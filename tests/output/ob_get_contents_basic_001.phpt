--TEST--
Test ob_get_contents() function : basic functionality
--CREDITS--
Iain Lewis <ilewis@php.net>
--FILE--
<?php
/* Prototype  : proto string ob_get_contents(void)
 * Description: Return the contents of the output buffer
 * Source code: main/output.c
 * Alias to functions:
 */


echo "*** Testing ob_get_contents() : basic functionality ***\n";

// Zero arguments
echo "\n-- Testing ob_get_contents() function with Zero arguments --\n";
/* Buffering not started yet, should return false */
var_dump( ob_get_contents() );

ob_start();
echo "Hello World\n";
$hello = ob_get_contents();
var_dump($hello);
ob_end_flush();


echo "\ncheck that we don't have a reference\n";
ob_start();
echo "Hello World\n";
$hello2 = ob_get_contents();
$hello2 = "bob";
var_dump(ob_get_contents());
ob_end_flush();

echo "\ncheck that contents disappear after a flush\n";
ob_start();
echo "Hello World\n";
ob_flush();
var_dump(ob_get_contents());
ob_end_flush();

echo "\ncheck that no contents found after an end\n";
ob_start();
echo "Hello World\n";
ob_end_flush();
var_dump(ob_get_contents());


echo "Done\n";
?>
--EXPECT--
*** Testing ob_get_contents() : basic functionality ***

-- Testing ob_get_contents() function with Zero arguments --
bool(false)
Hello World
string(12) "Hello World
"

check that we don't have a reference
Hello World
string(12) "Hello World
"

check that contents disappear after a flush
Hello World
string(0) ""

check that no contents found after an end
Hello World
bool(false)
Done
