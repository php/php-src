--TEST--
Test htmlspecialchars_decode() function : usage variations - double quoted strings for 'string' argument
--FILE--
<?php
/* Prototype  : string htmlspecialchars_decode(string $string [, int $quote_style])
 * Description: Convert special HTML entities back to characters
 * Source code: ext/standard/html.c
*/

/*
 * testing htmlspecialchars_decode() for various double quoted strings as argument for $string 
*/
echo "*** Testing htmlspecialchars_decode() : usage variations ***\n";

//double quoted strings
$strings = array (
  "Roy&#039s height &gt; Sam&#039;s \$height... 1111 &ap; 0000 = 0000... &quot; double quote string &quot;",
  "Roy&#039;s height &gt; Sam&#039;s height... \t\t 13 &lt; 15...\n\r &quot; double quote\f\v string &quot;",
  "\nRoy&#039;s height &gt\t; Sam&#039;s\v height\f",
  "\r\tRoy&#039;s height &gt\r; Sam\t&#039;s height",
  "\n 1\t3 &\tgt; 11 but 11 &\tlt; 12",
);
  
// loop through each element of the array to check htmlspecialchars_decode() function with all possible arguments
$iterator = 1;
foreach($strings as $value) {
      echo "-- Iteration $iterator --\n";
      var_dump( htmlspecialchars_decode($value) );
      var_dump( htmlspecialchars_decode($value, ENT_COMPAT) );
      var_dump( htmlspecialchars_decode($value, ENT_NOQUOTES) );
      var_dump( htmlspecialchars_decode($value, ENT_QUOTES) );
      $iterator++;
}

echo "Done";
?>
--EXPECTF--
*** Testing htmlspecialchars_decode() : usage variations ***
-- Iteration 1 --
string(89) "Roy&#039s height > Sam&#039;s $height... 1111 &ap; 0000 = 0000... " double quote string ""
string(89) "Roy&#039s height > Sam&#039;s $height... 1111 &ap; 0000 = 0000... " double quote string ""
string(99) "Roy&#039s height > Sam&#039;s $height... 1111 &ap; 0000 = 0000... &quot; double quote string &quot;"
string(84) "Roy&#039s height > Sam's $height... 1111 &ap; 0000 = 0000... " double quote string ""
-- Iteration 2 --
string(82) "Roy&#039;s height > Sam&#039;s height... 		 13 < 15...
 " double quote string ""
string(82) "Roy&#039;s height > Sam&#039;s height... 		 13 < 15...
 " double quote string ""
string(92) "Roy&#039;s height > Sam&#039;s height... 		 13 < 15...
 &quot; double quote string &quot;"
string(72) "Roy's height > Sam's height... 		 13 < 15...
 " double quote string ""
-- Iteration 3 --
string(44) "
Roy&#039;s height &gt	; Sam&#039;s height"
string(44) "
Roy&#039;s height &gt	; Sam&#039;s height"
string(44) "
Roy&#039;s height &gt	; Sam&#039;s height"
string(34) "
Roy's height &gt	; Sam's height"
-- Iteration 4 --
string(44) "	Roy&#039;s height &gt; Sam	&#039;s height"
string(44) "	Roy&#039;s height &gt; Sam	&#039;s height"
string(44) "	Roy&#039;s height &gt; Sam	&#039;s height"
string(34) "	Roy's height &gt; Sam	's height"
-- Iteration 5 --
string(30) "
 1	3 &	gt; 11 but 11 &	lt; 12"
string(30) "
 1	3 &	gt; 11 but 11 &	lt; 12"
string(30) "
 1	3 &	gt; 11 but 11 &	lt; 12"
string(30) "
 1	3 &	gt; 11 but 11 &	lt; 12"
Done