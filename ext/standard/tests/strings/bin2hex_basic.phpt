--TEST--
Test bin2hex() function : basic functionality 
--FILE--
<?php

/* Prototype  : string bin2hex  ( string $str  )
 * Description: Convert binary data into hexadecimal representation
 * Source code: ext/standard/string.c
*/

echo "*** Testing bin2hex() : basic functionality ***\n";

// array with different values for $string
$strings =  array (

		  //double quoted strings
/*1*/	  "Here is a simple string",
		  "\t This String contains \t\t some control characters\r\n",
		  "\x90\x91\x00\x93\x94\x90\x91\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f",
		  
		   //single quoted strings
/*4*/	  'Here is a simple string',
		  '\t This String contains \t\t some control characters\r\n',
		  '\x90\x91\x00\x93\x94\x90\x91\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f',
);  

// loop through with each element of the $strings array to test bin2hex() function
$count = 1;
foreach($strings as $string) {
  echo "-- Iteration $count --\n";	
  var_dump(bin2hex($string));
  $count ++;
}  
?>
===DONE===
--EXPECT--
*** Testing bin2hex() : basic functionality ***
-- Iteration 1 --
string(46) "4865726520697320612073696d706c6520737472696e67"
-- Iteration 2 --
string(102) "09205468697320537472696e6720636f6e7461696e7320090920736f6d6520636f6e74726f6c20636861726163746572730d0a"
-- Iteration 3 --
string(36) "9091009394909195969798999a9b9c9d9e9f"
-- Iteration 4 --
string(46) "4865726520697320612073696d706c6520737472696e67"
-- Iteration 5 --
string(112) "5c74205468697320537472696e6720636f6e7461696e73205c745c7420736f6d6520636f6e74726f6c20636861726163746572735c725c6e"
-- Iteration 6 --
string(144) "5c7839305c7839315c7830305c7839335c7839345c7839305c7839315c7839355c7839365c7839375c7839385c7839395c7839615c7839625c7839635c7839645c7839655c783966"
===DONE===
