--TEST--
Test print_r() function with strings
--WHITESPACE_SENSITIVE--
--FILE--
<?php

function check_printr( $variables ) {
    $counter = 1;
    foreach( $variables as $variable ) {
        echo "-- Iteration $counter --\n";
        print_r($variable, false);
        // $return=TRUE, print_r() will return its output, instead of printing it
        $ret_string = print_r($variable, true); //$ret_string captures the output
        echo "\n$ret_string\n";
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

check_printr($strings);

?>
--EXPECT--
-- Iteration 1 --


-- Iteration 2 --
 
 
-- Iteration 3 --
0
0
-- Iteration 4 --
\0
\0
-- Iteration 5 --
	
	
-- Iteration 6 --
\t
\t
-- Iteration 7 --
PHP
PHP
-- Iteration 8 --
1234	
5678
	9100"abcda
1234	
5678
	9100"abcda
