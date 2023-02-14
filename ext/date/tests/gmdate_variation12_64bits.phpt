--TEST--
Test gmdate() function : usage variation - Valid and invalid range of timestamp 64 bits.
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) die('skip 64 bit only');
?>
--FILE--
<?php
echo "*** Testing gmdate() : usage variation ***\n";

// Initialise all required variables
date_default_timezone_set('UTC');

$timestamp = mktime(20, 45, 54, 12, 13, 1901);
echo "\n-- Testing gmdate() function with minimum range of timestamp --\n";
var_dump( gmdate(DATE_ATOM, $timestamp) );

$timestamp = mktime(20, 45, 50, 12, 13, 1901);
echo "\n-- Testing gmdate() function with less than the range of timestamp --\n";
var_dump( gmdate(DATE_ATOM, $timestamp) );

echo "\n-- Testing gmdate() function with maximum range of timestamp --\n";
$timestamp = mktime(03, 14, 07, 1, 19, 2038);
var_dump( gmdate(DATE_ATOM, $timestamp) );

echo "\n-- Testing gmdate() function with greater than the range of timestamp --\n";
$timestamp = mktime(03, 14, 10, 1, 19, 2038);
var_dump( gmdate(DATE_ATOM, $timestamp) );

?>
--EXPECT--
*** Testing gmdate() : usage variation ***

-- Testing gmdate() function with minimum range of timestamp --
string(25) "1901-12-13T20:45:54+00:00"

-- Testing gmdate() function with less than the range of timestamp --
string(25) "1901-12-13T20:45:50+00:00"

-- Testing gmdate() function with maximum range of timestamp --
string(25) "2038-01-19T03:14:07+00:00"

-- Testing gmdate() function with greater than the range of timestamp --
string(25) "2038-01-19T03:14:10+00:00"
