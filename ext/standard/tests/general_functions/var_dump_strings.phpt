--TEST--
Test var_dump() function with strings
--WHITESPACE_SENSITIVE--
--FILE--
<?php

function check_var_dump( $variables ) {
    $counter = 1;
    foreach ($variables as $variable) {
        echo "-- Iteration $counter --\n";
        var_dump($variable);
        $counter++;
    }
}

$strings = [
  '',
  ' ',
  "0",
  '\0',
  "\t",
  '\t',
  'PHP',
  "1234\t\n5678\n\t9100\"abcda"  // strings with escape characters
];

check_var_dump($strings);

?>
--EXPECT--
-- Iteration 1 --
string(0) ""
-- Iteration 2 --
string(1) " "
-- Iteration 3 --
string(1) "0"
-- Iteration 4 --
string(2) "\0"
-- Iteration 5 --
string(1) "	"
-- Iteration 6 --
string(2) "\t"
-- Iteration 7 --
string(3) "PHP"
-- Iteration 8 --
string(22) "1234	
5678
	9100"abcda"
