--TEST--
Test str_split() function : usage variations - different heredoc strings as 'str' argument
--FILE--
<?php
/* Prototype  : array str_split(string $str [, int $split_length] )
 * Description: Convert a string to an array. If split_length is
                specified, break the string down into chunks each
                split_length characters long.
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Passing different heredoc strings as 'str' argument to the str_split()
* with 'split_length' 10
*/

echo "*** Testing str_split() : heredoc strings as 'str' argument ***\n";

// Initializing required variables
$split_length = 10;

// Null heredoc string
$heredoc_null = <<<EOT1
EOT1;

// heredoc string with single character
$heredoc_char = <<<EOT2
a
EOT2;

// simple heredoc string
$heredoc_str = <<<EOT3
This is simple heredoc string
EOT3;

// heredoc with special characters
$heredoc_spchar = <<<EOT4
This checks heredoc with $, %, &, chars
EOT4;

// blank heredoc string
$heredoc_blank = <<<EOT5

EOT5;

// heredoc with different white space characters
$heredoc_escchar = <<<EOT6
This checks\t str_split()\nEscape\rchars
EOT6;

// heredoc with multiline
$heredoc_multiline= <<<EOT7
This is to check str_split
function with multiline
heredoc
EOT7;

// heredoc with quotes and slashes
$heredoc_quote_slash = <<<EOT8
"To check " in heredoc"
I'm sure it'll work also with \
which is single slash
EOT8;

//different heredoc strings for 'str'
$heredoc_array = array(
  $heredoc_null,
  $heredoc_blank,
  $heredoc_char,
  $heredoc_str,
  $heredoc_multiline,
  $heredoc_spchar,
  $heredoc_escchar,
  $heredoc_quote_slash
);


// loop through each element of the 'heredoc_array' for 'str'
$count = 0;
foreach($heredoc_array as $str) {
  echo "-- Iteration ".($count+1). " --\n";
  var_dump( str_split($str, $split_length) );
  $count++;
};

echo "Done"
?>
--EXPECT--
*** Testing str_split() : heredoc strings as 'str' argument ***
-- Iteration 1 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 2 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 3 --
array(1) {
  [0]=>
  string(1) "a"
}
-- Iteration 4 --
array(3) {
  [0]=>
  string(10) "This is si"
  [1]=>
  string(10) "mple hered"
  [2]=>
  string(9) "oc string"
}
-- Iteration 5 --
array(6) {
  [0]=>
  string(10) "This is to"
  [1]=>
  string(10) " check str"
  [2]=>
  string(10) "_split
fun"
  [3]=>
  string(10) "ction with"
  [4]=>
  string(10) " multiline"
  [5]=>
  string(8) "
heredoc"
}
-- Iteration 6 --
array(4) {
  [0]=>
  string(10) "This check"
  [1]=>
  string(10) "s heredoc "
  [2]=>
  string(10) "with $, %,"
  [3]=>
  string(9) " &, chars"
}
-- Iteration 7 --
array(4) {
  [0]=>
  string(10) "This check"
  [1]=>
  string(10) "s	 str_spl"
  [2]=>
  string(10) "it()
Escap"
  [3]=>
  string(7) "echars"
}
-- Iteration 8 --
array(8) {
  [0]=>
  string(10) ""To check "
  [1]=>
  string(10) "" in hered"
  [2]=>
  string(10) "oc"
I'm su"
  [3]=>
  string(10) "re it'll w"
  [4]=>
  string(10) "ork also w"
  [5]=>
  string(10) "ith \
whic"
  [6]=>
  string(10) "h is singl"
  [7]=>
  string(7) "e slash"
}
Done
