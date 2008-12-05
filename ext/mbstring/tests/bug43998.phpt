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
  var_dump( mb_strtolower($sourcestring, $input) );
  var_dump( mb_strtoupper($sourcestring, $input) );
  $iterator++;
};
?>
--EXPECTF--

-- Iteration 1 --

Warning: mb_strtolower(): Unknown encoding "12345" in %s on line %d
bool(false)

Warning: mb_strtoupper(): Unknown encoding "12345" in %s on line %d
bool(false)

-- Iteration 2 --

Warning: mb_strtolower(): Unknown encoding "1.23456789E-9" in %s on line %d
bool(false)

Warning: mb_strtoupper(): Unknown encoding "1.23456789E-9" in %s on line %d
bool(false)

-- Iteration 3 --

Warning: mb_strtolower(): Unknown encoding "1" in %s on line %d
bool(false)

Warning: mb_strtoupper(): Unknown encoding "1" in %s on line %d
bool(false)

-- Iteration 4 --

Warning: mb_strtolower(): Unknown encoding "" in %s on line %d
bool(false)

Warning: mb_strtoupper(): Unknown encoding "" in %s on line %d
bool(false)

-- Iteration 5 --

Warning: mb_strtolower(): Unknown encoding "" in %s on line %d
bool(false)

Warning: mb_strtoupper(): Unknown encoding "" in %s on line %d
bool(false)
