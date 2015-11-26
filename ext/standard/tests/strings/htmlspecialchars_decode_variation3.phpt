--TEST--
Test htmlspecialchars_decode() function : usage variations - heredoc strings for 'string' argument
--FILE--
<?php
/* Prototype  : string htmlspecialchars_decode(string $string [, int $quote_style])
 * Description: Convert special HTML entities back to characters
 * Source code: ext/standard/html.c
*/

/* 
 * testing htmlspecialchars_decode() with various heredoc strings as argument for $string
*/

echo "*** Testing htmlspecialchars_decode() : usage variations ***\n";

// empty heredoc string
$empty_string = <<<EOT
EOT;

// Heredoc string with blank line
$blank_line = <<<EOT

EOT;

// heredoc with multiline string
$multiline_string = <<<EOT
<html>Roy&#039;s height &gt; Sam&#039;s height
13 &lt; 25
1111 &amp; 0000 = 0000
&quot;This is a double quoted string&quot;
EOT;

// heredoc with different whitespaces
$diff_whitespaces = <<<EOT
<html>Roy&#039;s height\r &gt; Sam\t&#039;s height
1111\t\t &amp; 0000\v\v = \f0000
&quot; heredoc\ndouble quoted string. with\vdifferent\fwhite\vspaces&quot;
EOT;

// heredoc with numeric values
$numeric_string = <<<EOT
<html>11 &lt; 12. 123 string 4567
&quot;string&quot; 1111\t &amp; 0000\t = 0000\n;
EOT;

// heredoc with quote chars & slash
$quote_char_string = <<<EOT
<html>&lt; This's a string with quotes:
"strings in double quote" &amp;
'strings in single quote' &quot;
this\line is &#039;single quoted&#039; /with\slashes </html>
EOT;

$res_heredoc_strings = array(
  //heredoc strings
  $empty_string,
  $blank_line,
  $multiline_string,
  $diff_whitespaces,
  $numeric_string,
  $quote_char_string
);

// loop through $res_heredoc_strings array and check the working on htmlspecialchars_decode()
$count = 1;
for($index =0; $index < count($res_heredoc_strings); $index ++) {
  echo "-- Iteration $count --\n";
  var_dump( htmlspecialchars_decode($res_heredoc_strings[$index]) );
  $count++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing htmlspecialchars_decode() : usage variations ***
-- Iteration 1 --
string(0) ""
-- Iteration 2 --
string(0) ""
-- Iteration 3 --
string(103) "<html>Roy&#039;s height > Sam&#039;s height
13 < 25
1111 & 0000 = 0000
"This is a double quoted string""
-- Iteration 4 --
string(130) "<html>Roy&#039;s height > Sam	&#039;s height
1111		 & 0000 = 0000
" heredoc
double quoted string. withdifferentwhitespaces""
-- Iteration 5 --
string(62) "<html>11 < 12. 123 string 4567
"string" 1111	 & 0000	 = 0000
;"
-- Iteration 6 --
string(153) "<html>< This's a string with quotes:
"strings in double quote" &
'strings in single quote' "
this\line is &#039;single quoted&#039; /with\slashes </html>"
Done