--TEST--
Test gmdate() function : usage variation - Passing Month format options to format argument.
--FILE--
<?php
echo "*** Testing gmdate() : usage variation ***\n";

// Initialise all required variables
date_default_timezone_set('UTC');
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

echo "\n-- Testing gmdate() function with full textual representation of month format --\n";
var_dump( gmdate('F') );
var_dump( gmdate('F', $timestamp) );

echo "\n-- Testing gmdate() function with numeric representation of month format --\n";
var_dump( gmdate('m') );
var_dump( gmdate('m', $timestamp) );

echo "\n-- Testing gmdate() function with short textual representation of month format --\n";
var_dump( gmdate('M') );
var_dump( gmdate('M', $timestamp) );

echo "\n-- Testing gmdate() function with numeric representation of month without leading zeros format --\n";
var_dump( gmdate('n') );
var_dump( gmdate('n', $timestamp) );

echo "\n-- Testing gmdate() function with number of days in a month format --\n";
var_dump( gmdate('t') );
var_dump( gmdate('t', $timestamp) );

?>
--EXPECTF--
*** Testing gmdate() : usage variation ***

-- Testing gmdate() function with full textual representation of month format --
string(%d) "%s"
string(6) "August"

-- Testing gmdate() function with numeric representation of month format --
string(%d) "%d"
string(2) "08"

-- Testing gmdate() function with short textual representation of month format --
string(%d) "%s"
string(3) "Aug"

-- Testing gmdate() function with numeric representation of month without leading zeros format --
string(%d) "%d"
string(1) "8"

-- Testing gmdate() function with number of days in a month format --
string(%d) "%d"
string(2) "31"
