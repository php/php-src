--TEST--
Test str_split() function : usage variations - different single quoted strings for 'str' argument
--FILE--
<?php
/*
* passing different single quoted strings as 'str' argument to str_split()
* split_length is set to 5
*/

echo "*** Testing str_split() : single quoted strings for 'str' ***\n";

//Initialize variables
$split_length = 5;

// different values for 'str'
$values = array(
  '',  //empty
  ' ',  //space
  '1234', //with only numbers
  'simple string',  //regular string
  'It\'s string with quote',  //string containing single quote
  'string\tcontains\rwhite space\nchars',
  'containing @ # $ % ^ & chars',
  'with 1234 numbers',
  'with \0 and ".chr(0)."null chars',  //for binary safe
  'with    multiple     space char',
  'Testing invalid \k and \m escape char',
  'to check with \\n and \\t' //ignoring \n and \t results
);

//loop through each element of $values for 'str' argument
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count+1)." --\n";
  var_dump( str_split($values[$count], $split_length) );
}
echo "Done"
?>
--EXPECT--
*** Testing str_split() : single quoted strings for 'str' ***
-- Iteration 1 --
array(1) {
  [0]=>
  string(0) ""
}
-- Iteration 2 --
array(1) {
  [0]=>
  string(1) " "
}
-- Iteration 3 --
array(1) {
  [0]=>
  string(4) "1234"
}
-- Iteration 4 --
array(3) {
  [0]=>
  string(5) "simpl"
  [1]=>
  string(5) "e str"
  [2]=>
  string(3) "ing"
}
-- Iteration 5 --
array(5) {
  [0]=>
  string(5) "It's "
  [1]=>
  string(5) "strin"
  [2]=>
  string(5) "g wit"
  [3]=>
  string(5) "h quo"
  [4]=>
  string(2) "te"
}
-- Iteration 6 --
array(8) {
  [0]=>
  string(5) "strin"
  [1]=>
  string(5) "g\tco"
  [2]=>
  string(5) "ntain"
  [3]=>
  string(5) "s\rwh"
  [4]=>
  string(5) "ite s"
  [5]=>
  string(5) "pace\"
  [6]=>
  string(5) "nchar"
  [7]=>
  string(1) "s"
}
-- Iteration 7 --
array(6) {
  [0]=>
  string(5) "conta"
  [1]=>
  string(5) "ining"
  [2]=>
  string(5) " @ # "
  [3]=>
  string(5) "$ % ^"
  [4]=>
  string(5) " & ch"
  [5]=>
  string(3) "ars"
}
-- Iteration 8 --
array(4) {
  [0]=>
  string(5) "with "
  [1]=>
  string(5) "1234 "
  [2]=>
  string(5) "numbe"
  [3]=>
  string(2) "rs"
}
-- Iteration 9 --
array(7) {
  [0]=>
  string(5) "with "
  [1]=>
  string(5) "\0 an"
  [2]=>
  string(5) "d ".c"
  [3]=>
  string(5) "hr(0)"
  [4]=>
  string(5) "."nul"
  [5]=>
  string(5) "l cha"
  [6]=>
  string(2) "rs"
}
-- Iteration 10 --
array(7) {
  [0]=>
  string(5) "with "
  [1]=>
  string(5) "   mu"
  [2]=>
  string(5) "ltipl"
  [3]=>
  string(5) "e    "
  [4]=>
  string(5) " spac"
  [5]=>
  string(5) "e cha"
  [6]=>
  string(1) "r"
}
-- Iteration 11 --
array(8) {
  [0]=>
  string(5) "Testi"
  [1]=>
  string(5) "ng in"
  [2]=>
  string(5) "valid"
  [3]=>
  string(5) " \k a"
  [4]=>
  string(5) "nd \m"
  [5]=>
  string(5) " esca"
  [6]=>
  string(5) "pe ch"
  [7]=>
  string(2) "ar"
}
-- Iteration 12 --
array(5) {
  [0]=>
  string(5) "to ch"
  [1]=>
  string(5) "eck w"
  [2]=>
  string(5) "ith \"
  [3]=>
  string(5) "n and"
  [4]=>
  string(3) " \t"
}
Done
