--TEST--
Test convert_uudecode() function : basic functionality
--FILE--
<?php

/* Prototype  : string convert_uudecode  ( string $data  )
 * Description: Decode a uuencoded string
 * Source code: ext/standard/uuencode.c
*/

echo "*** Testing convert_uudecode() : basic functionality ***\n";

// array with different values for $string
$strings =  array (

  //double quoted strings
  "123",
  "abc",
  "1a2b3c",
  "Here is a simple string to test convert_uuencode/decode",
  "\t This String contains \t\t some control characters\r\n",
  "\x90\x91\x00\x93\x94\x90\x91\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f",

   //single quoted strings
  '123',
  'abc',
  '1a2b3c',
  '\t This String contains \t\t some control characters\r\n',

);

// loop through with each element of the $strings array to test convert_uudecode() function
$count = 1;
foreach($strings as $string) {

  $encode = convert_uuencode($string);
  $decode = convert_uudecode($encode);

  if ($decode != $string) {
  	var_dump($encode, $decode, $string);
  	exit("TEST FAILED on iteration $count\n");
  }

  $count ++;
}

echo "TEST PASSED\n";

?>
===DONE===
--EXPECT--
*** Testing convert_uudecode() : basic functionality ***
TEST PASSED
===DONE===
