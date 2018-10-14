--TEST--
Test htmlspecialchars_decode() function : usage variations - single quoted strings for 'string' argument
--FILE--
<?php
/* Prototype  : string htmlspecialchars_decode(string $string [, int $quote_style])
 * Description: Convert special HTML entities back to characters
 * Source code: ext/standard/html.c
*/

/*
 * Testing htmlspecialchars_decode() with various single quoted strings as argument for $string
*/

echo "*** Testing htmlspecialchars_decode() : usage variations ***\n";

//single quoted strings
$values = array (
  'Roy&#039s height &gt; Sam&#039;s \$height... 1111 &ap; 0000 = 0000... &quot; double quote string &quot;',
  'Roy&#039;s height &gt; Sam&#039;s height... \t\t 13 &lt; 15...\n\r &quot; double quote\f\v string &quot;',
  '\nRoy&#039;s height &gt\t; Sam&#039;s\v height\f',
  '\r\tRoy&#039;s height &gt\r; Sam\t&#039;s height',
  '\n 1\t3 &\tgt; 11 but 11 &\tlt; 12',
);

// loop through each element of the values array to check htmlspecialchars_decode() function with all possible arguments
$iterator = 1;
foreach($values as $value) {
      echo "-- Iteration $iterator --\n";
      var_dump( htmlspecialchars_decode($value) );
      var_dump( htmlspecialchars_decode($value, ENT_COMPAT) );
      var_dump( htmlspecialchars_decode($value, ENT_NOQUOTES) );
      var_dump( htmlspecialchars_decode($value, ENT_QUOTES) );
      $iterator++;
}

echo "Done";
?>
--EXPECT--
*** Testing htmlspecialchars_decode() : usage variations ***
-- Iteration 1 --
string(90) "Roy&#039s height > Sam&#039;s \$height... 1111 &ap; 0000 = 0000... " double quote string ""
string(90) "Roy&#039s height > Sam&#039;s \$height... 1111 &ap; 0000 = 0000... " double quote string ""
string(100) "Roy&#039s height > Sam&#039;s \$height... 1111 &ap; 0000 = 0000... &quot; double quote string &quot;"
string(85) "Roy&#039s height > Sam's \$height... 1111 &ap; 0000 = 0000... " double quote string ""
-- Iteration 2 --
string(88) "Roy&#039;s height > Sam&#039;s height... \t\t 13 < 15...\n\r " double quote\f\v string ""
string(88) "Roy&#039;s height > Sam&#039;s height... \t\t 13 < 15...\n\r " double quote\f\v string ""
string(98) "Roy&#039;s height > Sam&#039;s height... \t\t 13 < 15...\n\r &quot; double quote\f\v string &quot;"
string(78) "Roy's height > Sam's height... \t\t 13 < 15...\n\r " double quote\f\v string ""
-- Iteration 3 --
string(48) "\nRoy&#039;s height &gt\t; Sam&#039;s\v height\f"
string(48) "\nRoy&#039;s height &gt\t; Sam&#039;s\v height\f"
string(48) "\nRoy&#039;s height &gt\t; Sam&#039;s\v height\f"
string(38) "\nRoy's height &gt\t; Sam's\v height\f"
-- Iteration 4 --
string(48) "\r\tRoy&#039;s height &gt\r; Sam\t&#039;s height"
string(48) "\r\tRoy&#039;s height &gt\r; Sam\t&#039;s height"
string(48) "\r\tRoy&#039;s height &gt\r; Sam\t&#039;s height"
string(38) "\r\tRoy's height &gt\r; Sam\t's height"
-- Iteration 5 --
string(34) "\n 1\t3 &\tgt; 11 but 11 &\tlt; 12"
string(34) "\n 1\t3 &\tgt; 11 but 11 &\tlt; 12"
string(34) "\n 1\t3 &\tgt; 11 but 11 &\tlt; 12"
string(34) "\n 1\t3 &\tgt; 11 but 11 &\tlt; 12"
Done
