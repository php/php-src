--TEST--
Test mb_strtolower() function : Two error messages returned for incorrect encoding for mb_strto[upper|lower]
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strtolower') or die("skip mb_strtolower() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strto[lower|upper](string $sourcestring [, string $encoding])
 * Description: Returns a [lower|upper]cased version of $sourcestring
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Two error messages returned for incorrect encoding for mb_strto[upper|lower]
 * Bug now appears to be fixed
 */

$sourcestring = 'Hello, World';

$inputs = array(12345, 12.3456789000E-10, true, false, "");
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
	try {
        var_dump( mb_strtolower($sourcestring, $input) );
	} catch (\ValueError $e) {
	    echo $e->getMessage() . \PHP_EOL;
	}
    try {
        var_dump( mb_strtoupper($sourcestring, $input) );
	} catch (\ValueError $e) {
	    echo $e->getMessage() . \PHP_EOL;
	}
    $iterator++;
};
?>
--EXPECT--
-- Iteration 1 --
Unknown encoding "12345"
Unknown encoding "12345"

-- Iteration 2 --
Unknown encoding "1.23456789E-9"
Unknown encoding "1.23456789E-9"

-- Iteration 3 --
Unknown encoding "1"
Unknown encoding "1"

-- Iteration 4 --
Unknown encoding ""
Unknown encoding ""

-- Iteration 5 --
Unknown encoding ""
Unknown encoding ""
