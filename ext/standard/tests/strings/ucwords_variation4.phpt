--TEST--
Test ucwords() function : usage variations - double quoted string
--FILE--
<?php
/* Prototype  : string ucwords ( string $str )
 * Description: Uppercase the first character of each word in a string
 * Source code: ext/standard/string.c
*/

/*
 * test ucwords() with different string prepared using double quote
*/

echo "*** Testing ucwords() : usage variations ***\n";

// different strings containing regular chars and special chars
$str_array = array(
  // multiple spaces
  "testing    ucwords",
  "t e s t i n g   u c w o r d s ",

  // brackets in sentence
  "testing function(ucwords)",
  "(testing ( function (ucwords) )a )test",
  "(t)",
  " ( t )t",

  // using quote chars in sentence
  "\"testing\",ucwords,\"test\"",
  "\"t\"\"t\",test, t",
  "\'t \'t\',test",
  "Jack's pen",
  "P't'y 't it's ",
  
  // using other white spaces
  "\ttesting\ttesting\tucwords",
  "\\ttesting\\ttesting\tucwords",
  "testing\rucwords testing ucwords",
  "testing\\rucwords testing ucwords",
  "testing\fucwords \f testing \nucwords",
  "testing\\fucwords \\f testing \nucwords",
  "\ntesting\nucwords\n testing \n ucwords",
  "\\ntesting\\nucwords\\n testing \\n ucwords",
  "using\vvertical\vtab",
  "using\\vvertical\\vtab",

  //using special chars in sentence
  "t@@#$% %test ^test &test *test +test -test",
  "!test ~test `test` =test= @test@test.com",
  "/test/r\test\ucwords\t\y\y\u\3 \yy\ /uu/",
  
  //only special chars
  "!@#$%^&*()_+=-`~"
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
string(24) ""testing",ucwords,"test""
-- Iteration 8 --
string(14) ""t""t",test, T"
-- Iteration 9 --
string(14) "\'t \'t\',test"
-- Iteration 10 --
string(10) "Jack's Pen"
-- Iteration 11 --
string(14) "P't'y 't It's "
-- Iteration 12 --
string(24) "	Testing	Testing	Ucwords"
-- Iteration 13 --
string(26) "\ttesting\ttesting	Ucwords"
-- Iteration 14 --
string(31) "TestingUcwords Testing Ucwords"
-- Iteration 15 --
string(32) "Testing\rucwords Testing Ucwords"
-- Iteration 16 --
string(34) "TestingUcwords  Testing 
Ucwords"
-- Iteration 17 --
string(36) "Testing\fucwords \f Testing 
Ucwords"
-- Iteration 18 --
string(35) "
Testing
Ucwords
 Testing 
 Ucwords"
-- Iteration 19 --
string(39) "\ntesting\nucwords\n Testing \n Ucwords"
-- Iteration 20 --
string(18) "UsingVerticalTab"
-- Iteration 21 --
string(20) "Using\vvertical\vtab"
-- Iteration 22 --
string(42) "T@@#$% %test ^test &test *test +test -test"
-- Iteration 23 --
string(40) "!test ~test `test` =test= @test@test.com"
-- Iteration 24 --
string(37) "/test/r	Est\ucwords	\y\y\u \yy\ /uu/"
-- Iteration 25 --
string(16) "!@#$%^&*()_+=-`~"
Done
