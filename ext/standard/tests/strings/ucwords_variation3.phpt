--TEST--
Test ucwords() function : usage variations - single quoted string
--FILE--
<?php
/* Prototype  : string ucwords ( string $str )
 * Description: Uppercase the first character of each word in a string
 * Source code: ext/standard/string.c
*/

/*
 * test ucwords() with different string prepared using single quote
*/

echo "*** Testing ucwords() : usage variations ***\n";

// different strings containing regular chars and special chars
$str_array = array(
  // multiple spaces
  'testing    ucwords',
  't e s t i n g   u c w o r d s ',

  // brackets in sentence
  'testing function(ucwords)',
  '(testing ( function (ucwords) )a )test',
  '(t)',
  ' ( t )t',

  // using quote chars in sentence
  '"testing",ucword,"test"',
  '"t""t",test, t',
  '\'t \'t\',test',

  // using other white spaces
  '\ttesting\ttesting\tucwords',
  'testing\rucwords testing ucwords',
  'testing\fucwords \f testing \nucwords',
  '\ntesting\nucwords\n testing \n ucwords',
  'using\vvertical\vtab',

  //using special chars in sentence
  't@@#$% %test ^test &test *test +test -test',
  '!test ~test `test` =test= @test@test.com',
  '/test/r\test\ucwords\t\y\y\u\3 \yy\ /uu/',

  //only special chars
  '!@#$%^&*()_+=-`~'
);

// loop through the $str_array array to test ucwords on each element
$iteration = 1;
for($index = 0; $index < count($str_array); $index++) {
  echo "-- Iteration $iteration --\n";
  var_dump( ucwords($str_array[$index]) );
  $iteration++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing ucwords() : usage variations ***
-- Iteration 1 --
string(18) "Testing    Ucwords"
-- Iteration 2 --
string(30) "T E S T I N G   U C W O R D S "
-- Iteration 3 --
string(25) "Testing Function(ucwords)"
-- Iteration 4 --
string(38) "(testing ( Function (ucwords) )a )test"
-- Iteration 5 --
string(3) "(t)"
-- Iteration 6 --
string(7) " ( T )t"
-- Iteration 7 --
string(23) ""testing",ucword,"test""
-- Iteration 8 --
string(14) ""t""t",test, T"
-- Iteration 9 --
string(11) "'t 't',test"
-- Iteration 10 --
string(27) "\ttesting\ttesting\tucwords"
-- Iteration 11 --
string(32) "Testing\rucwords Testing Ucwords"
-- Iteration 12 --
string(37) "Testing\fucwords \f Testing \nucwords"
-- Iteration 13 --
string(39) "\ntesting\nucwords\n Testing \n Ucwords"
-- Iteration 14 --
string(20) "Using\vvertical\vtab"
-- Iteration 15 --
string(42) "T@@#$% %test ^test &test *test +test -test"
-- Iteration 16 --
string(40) "!test ~test `test` =test= @test@test.com"
-- Iteration 17 --
string(40) "/test/r\test\ucwords\t\y\y\u\3 \yy\ /uu/"
-- Iteration 18 --
string(16) "!@#$%^&*()_+=-`~"
Done
