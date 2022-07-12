--TEST--
Test str_split() function : usage variations - different double quoted strings for 'str' argument
--FILE--
<?php
/*
* passing different double quoted strings as 'str' argument to str_split()
* split_length is set to 7
*/

echo "*** Testing str_split() : double quoted strings for 'str' ***\n";

//Initialize variables
$split_length = 7;

// different values for 'str'
$values = array(
  "",  //empty
  " ",  //space
  "1234", //with only numbers
  "simple string",  //regular string
  "It's string with quote",  //string containing single quote
  "string\tcontains\rwhite space\nchars",
  "containing @ # $ % ^ & chars",
  "with 1234 numbers",
  "with \0 and ".chr(0)."null chars",  //for binary safe
  "with    multiple     space char",
  "Testing invalid \k and \m escape char",
  "to check with \\n and \\t" //ignoring \n and \t results

);

//loop through each element of $values for 'str' argument
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count+1)." --\n";
  var_dump( str_split($values[$count], $split_length) );
}
echo "Done"
?>
--EXPECTF--
*** Testing str_split() : double quoted strings for 'str' ***
-- Iteration 1 --
array(0) {
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
array(2) {
  [0]=>
  string(7) "simple "
  [1]=>
  string(6) "string"
}
-- Iteration 5 --
array(4) {
  [0]=>
  string(7) "It's st"
  [1]=>
  string(7) "ring wi"
  [2]=>
  string(7) "th quot"
  [3]=>
  string(1) "e"
}
-- Iteration 6 --
array(5) {
  [0]=>
  string(7) "string	"
  [1]=>
  string(7) "contain"
  [2]=>
  string(7) "swhite"
  [3]=>
  string(7) " space
"
  [4]=>
  string(5) "chars"
}
-- Iteration 7 --
array(4) {
  [0]=>
  string(7) "contain"
  [1]=>
  string(7) "ing @ #"
  [2]=>
  string(7) " $ % ^ "
  [3]=>
  string(7) "& chars"
}
-- Iteration 8 --
array(3) {
  [0]=>
  string(7) "with 12"
  [1]=>
  string(7) "34 numb"
  [2]=>
  string(3) "ers"
}
-- Iteration 9 --
array(4) {
  [0]=>
  string(7) "with %0 "
  [1]=>
  string(7) "and %0nu"
  [2]=>
  string(7) "ll char"
  [3]=>
  string(1) "s"
}
-- Iteration 10 --
array(5) {
  [0]=>
  string(7) "with   "
  [1]=>
  string(7) " multip"
  [2]=>
  string(7) "le     "
  [3]=>
  string(7) "space c"
  [4]=>
  string(3) "har"
}
-- Iteration 11 --
array(6) {
  [0]=>
  string(7) "Testing"
  [1]=>
  string(7) " invali"
  [2]=>
  string(7) "d \k an"
  [3]=>
  string(7) "d \m es"
  [4]=>
  string(7) "cape ch"
  [5]=>
  string(2) "ar"
}
-- Iteration 12 --
array(4) {
  [0]=>
  string(7) "to chec"
  [1]=>
  string(7) "k with "
  [2]=>
  string(7) "\n and "
  [3]=>
  string(2) "\t"
}
Done
