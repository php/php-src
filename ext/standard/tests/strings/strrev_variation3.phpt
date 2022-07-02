--TEST--
Test strrev() function : usage variations - heredoc strings
--FILE--
<?php
/* Testing strrev() function with heredoc strings for 'str' */

echo "*** Testing strrev() function: with heredoc strings ***\n";
$multi_line_str = <<<EOD
Example of string
spanning multiple lines
using heredoc syntax.
EOD;

$special_chars_str = <<<EOD
Ex'ple of h'doc st'g, contains
$#%^*&*_("_")!#@@!$#$^^&*(special)
chars.
EOD;

$control_chars_str = <<<EOD
Hello, World\n
Hello\0World
EOD;

$quote_chars_str = <<<EOD
it's bright o'side
"things in double quote"
'things in single quote'
this\line is /with\slashs
EOD;

$blank_line = <<<EOD

EOD;

$empty_str = <<<EOD
EOD;

$strings = array(
  $multi_line_str,
  $special_chars_str,
  $control_chars_str,
  $quote_chars_str,
  $blank_line,
  $empty_str
);

$count = 1;
for( $index = 0; $index < count($strings); $index++ ) {
  echo "\n-- Iteration $count --\n";
  var_dump( strrev($strings[$index]) );
  $count ++;
}

echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrev() function: with heredoc strings ***

-- Iteration 1 --
string(63) ".xatnys codereh gnisu
senil elpitlum gninnaps
gnirts fo elpmaxE"

-- Iteration 2 --
string(72) ".srahc
)laiceps(*&^^$#$!@@#!)"_"(_*&*^%#$
sniatnoc ,g'ts cod'h fo elp'xE"

-- Iteration 3 --
string(25) "dlroW%0olleH

dlroW ,olleH"

-- Iteration 4 --
string(94) "shsals\htiw/ si enil\siht
'etouq elgnis ni sgniht'
"etouq elbuod ni sgniht"
edis'o thgirb s'ti"

-- Iteration 5 --
string(0) ""

-- Iteration 6 --
string(0) ""
*** Done ***
