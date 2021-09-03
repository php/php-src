--TEST--
Testing substr() function
--FILE--
<?php

$strings_array = array( "", 12345, "abcdef", "123abc", "_123abc");

$counter = 1;
foreach ($strings_array as $str) {
  /* variations with two arguments */
  /* start values >, < and = 0    */

  echo ("\n--- Iteration ".$counter." ---\n");
  echo ("\n-- Variations for two arguments --\n");
  var_dump ( substr($str, 1) );
  var_dump ( substr($str, 0) );
  var_dump ( substr($str, -2) );

  /* variations with three arguments */
  /* start value variations with length values  */

  echo ("\n-- Variations for three arguments --\n");
  var_dump ( substr($str, 1, 3) );
  var_dump ( substr($str, 1, 0) );
  var_dump ( substr($str, 1, -3) );
  var_dump ( substr($str, 0, 3) );
  var_dump ( substr($str, 0, 0) );
  var_dump ( substr($str, 0, -3) );
  var_dump ( substr($str, -2, 3) );
  var_dump ( substr($str, -2, 0 ) );
  var_dump ( substr($str, -2, -3) );

  $counter++;
}

/* variation of start and length to point to same element */
echo ("\n*** Testing for variations of start and length to point to same element ***\n");
var_dump (substr("abcde" , 2, -2) );
var_dump (substr("abcde" , -3, -2) );

/* Testing to return empty string when start denotes the position beyond the truncation (set by negative length) */
echo ("\n*** Testing for start > truncation  ***\n");
var_dump (substr("abcdef" , 4, -4) );

/* String with null character */
echo ("\n*** Testing for string with null characters ***\n");
var_dump (substr("abc\x0xy\x0z" ,2) );

/* String with international characters */
echo ("\n*** Testing for string with international characters ***\n");
var_dump (substr('\xIñtërnâtiônàlizætiøn',3) );

/* start <0 && -start > length */
echo "\n*** Start before the first char ***\n";
var_dump (substr("abcd" , -8) );

/* Omitting length and passing a NULL length */
echo "\n*** Omitting length or using NULL length ***\n";
var_dump (substr("abcdef" , 2) );
var_dump (substr("abcdef" , 2, NULL) );

echo"\nDone";

?>
--EXPECTF--
--- Iteration 1 ---

-- Variations for two arguments --
string(0) ""
string(0) ""
string(0) ""

-- Variations for three arguments --
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""

--- Iteration 2 ---

-- Variations for two arguments --
string(4) "2345"
string(5) "12345"
string(2) "45"

-- Variations for three arguments --
string(3) "234"
string(0) ""
string(1) "2"
string(3) "123"
string(0) ""
string(2) "12"
string(2) "45"
string(0) ""
string(0) ""

--- Iteration 3 ---

-- Variations for two arguments --
string(5) "bcdef"
string(6) "abcdef"
string(2) "ef"

-- Variations for three arguments --
string(3) "bcd"
string(0) ""
string(2) "bc"
string(3) "abc"
string(0) ""
string(3) "abc"
string(2) "ef"
string(0) ""
string(0) ""

--- Iteration 4 ---

-- Variations for two arguments --
string(5) "23abc"
string(6) "123abc"
string(2) "bc"

-- Variations for three arguments --
string(3) "23a"
string(0) ""
string(2) "23"
string(3) "123"
string(0) ""
string(3) "123"
string(2) "bc"
string(0) ""
string(0) ""

--- Iteration 5 ---

-- Variations for two arguments --
string(6) "123abc"
string(7) "_123abc"
string(2) "bc"

-- Variations for three arguments --
string(3) "123"
string(0) ""
string(3) "123"
string(3) "_12"
string(0) ""
string(4) "_123"
string(2) "bc"
string(0) ""
string(0) ""

*** Testing for variations of start and length to point to same element ***
string(1) "c"
string(1) "c"

*** Testing for start > truncation  ***
string(0) ""

*** Testing for string with null characters ***
string(6) "c%0xy%0z"

*** Testing for string with international characters ***
string(26) "ñtërnâtiônàlizætiøn"

*** Start before the first char ***
string(4) "abcd"

*** Omitting length or using NULL length ***
string(4) "cdef"
string(4) "cdef"

Done
