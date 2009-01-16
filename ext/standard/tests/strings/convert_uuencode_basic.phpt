--TEST--
Test convert_uuencode() function : basic functionality 
--FILE--
<?php

/* Prototype  : string convert_uuencode  ( string $data  )
 * Description: Uuencode a string
 * Source code: ext/standard/uuencode.c
*/

echo "*** Testing convert_uuencode() : basic functionality ***\n";

// array with different values for $string
$strings =  array (

  //double quoted strings
  b"123",
  b"abc",
  b"1a2b3c",
  b"Here is a simple string to test convert_uuencode/decode",
  b"\t This String contains \t\t some control characters\r\n",
  b"\x90\x91\x00\x93\x94\x90\x91\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f",
  
   //single quoted strings
  b'123',
  b'abc',
  b'1a2b3c',
  b'\t This String contains \t\t some control characters\r\n',
  
);  

// loop through with each element of the $strings array to test convert_uuencode() function
$count = 1;
foreach($strings as $string) {
  echo "-- Iteration $count --\n";
  var_dump( convert_uuencode($string) );
  $count ++;
}


?>
===DONE=== 
--EXPECTF--
*** Testing convert_uuencode() : basic functionality ***
-- Iteration 1 --
string(8) "#,3(S
`
"
-- Iteration 2 --
string(8) "#86)C
`
"
-- Iteration 3 --
string(12) "&,6$R8C-C
`
"
-- Iteration 4 --
string(82) "M2&5R92!I<R!A('-I;7!L92!S=')I;F<@=&\@=&5S="!C;VYV97)T7W5U96YC
*;V1E+V1E8V]D90``
`
"
-- Iteration 5 --
string(74) "M"2!4:&ES(%-T<FEN9R!C;VYT86EN<R`)"2!S;VUE(&-O;G1R;VP@8VAA<F%C
&=&5R<PT*
`
"
-- Iteration 6 --
string(28) "2D)$`DY20D966EYB9FIN<G9Z?
`
"
-- Iteration 7 --
string(8) "#,3(S
`
"
-- Iteration 8 --
string(8) "#86)C
`
"
-- Iteration 9 --
string(12) "&,6$R8C-C
`
"
-- Iteration 10 --
string(82) "M7'0@5&AI<R!3=')I;F<@8V]N=&%I;G,@7'1<="!S;VUE(&-O;G1R;VP@8VAA
+<F%C=&5R<UQR7&X`
`
"
===DONE===
