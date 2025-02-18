--TEST--
print_r(): Test printing strings
--WHITESPACE_SENSITIVE--
--FILE--
<?php

function check_print_r($variables) {
    $counter = 1;
    foreach($variables as $variable ) {
        echo "-- Iteration $counter --\n";
        ob_start();
        $should_be_true = print_r($variable, /* $return */ false);
        $output_content = ob_get_flush();
        echo PHP_EOL;
        var_dump($should_be_true);
        $print_r_with_return = print_r($variable, /* $return */ true);
        var_dump($output_content === $print_r_with_return);
        $output_content = null;
        $counter++;
    }
}

$strings = [
  '',
  ' ',
  '0',
  'PHP',
  // strings with escape characters
  "12\t34\n5678\n\t9100\"abcda",
];

check_print_r($strings);

?>
--EXPECT--
-- Iteration 1 --

bool(true)
bool(true)
-- Iteration 2 --
 
bool(true)
bool(true)
-- Iteration 3 --
0
bool(true)
bool(true)
-- Iteration 4 --
PHP
bool(true)
bool(true)
-- Iteration 5 --
12	34
5678
	9100"abcda
bool(true)
bool(true)
