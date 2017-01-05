--TEST--
Test getrusage() function : error conditions - incorrect number of args
--SKIPIF--
<?php if (!function_exists("getrusage")) print "skip"; ?>
--FILE--
<?php
/* Prototype  :  array getrusage  ([ int $who  ] )
 * Description: Gets the current resource usages
 * Source code: ext/standard/microtime.c
 * Alias to functions: 
 */

/*
 * Pass an incorrect number of arguments to getrusage() to test behaviour
 */

echo "*** Testing getrusage() : error conditions ***\n";

echo "\n-- Testing getrusage() function with more than expected no. of arguments --\n";
$extra_arg = 10;
$dat = getrusage(1, $extra_arg);

echo "\n-- Testing getrusage() function with invalid argument - non-numeric STRING--\n";
$string_arg = "foo";
$dat = getrusage($string_arg);

echo "\n-- Testing getrusage() function with invalid argument - ARRAY--\n";
$array_arg = array(1,2,3);
$dat = getrusage($array_arg);

echo "\n-- Testing getrusage() function with invalid argument - OBJECT --\n";
class classA 
{
  function __toString() {
    return "ClassAObject";
  }
}
$obj_arg = new classA();
$dat = getrusage($obj_arg);

echo "\n-- Testing getrusage() function with invalid argument - RESOURCE --\n";
$file_handle=fopen(__FILE__, "r");
$dat = getrusage($file_handle);
fclose($file_handle);

?>
===DONE===
--EXPECTF--
*** Testing getrusage() : error conditions ***

-- Testing getrusage() function with more than expected no. of arguments --

Warning: getrusage() expects at most 1 parameter, 2 given in %s on line %d

-- Testing getrusage() function with invalid argument - non-numeric STRING--

Warning: getrusage() expects parameter 1 to be integer, string given in %s on line %d

-- Testing getrusage() function with invalid argument - ARRAY--

Warning: getrusage() expects parameter 1 to be integer, array given in %s on line %d

-- Testing getrusage() function with invalid argument - OBJECT --

Warning: getrusage() expects parameter 1 to be integer, object given in %s on line %d

-- Testing getrusage() function with invalid argument - RESOURCE --

Warning: getrusage() expects parameter 1 to be integer, resource given in %s on line %d
===DONE===
