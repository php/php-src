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
  "/test/r\test\ ucwords\t\y\y\3 \yy\ /uu/",
  
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
--EXPECT--
*** Testing ucwords() : usage variations ***
-- Iteration 1 --
unicode(18) "Testing    Ucwords"
-- Iteration 2 --
unicode(30) "T E S T I N G   U C W O R D S "
-- Iteration 3 --
unicode(25) "Testing Function(ucwords)"
-- Iteration 4 --
unicode(38) "(testing ( Function (ucwords) )a )test"
-- Iteration 5 --
unicode(3) "(t)"
-- Iteration 6 --
unicode(7) " ( T )t"
-- Iteration 7 --
unicode(24) ""testing",ucwords,"test""
-- Iteration 8 --
unicode(14) ""t""t",test, T"
-- Iteration 9 --
unicode(14) "\'t \'t\',test"
-- Iteration 10 --
unicode(10) "Jack's Pen"
-- Iteration 11 --
unicode(14) "P't'y 't It's "
-- Iteration 12 --
unicode(24) "	Testing	Testing	Ucwords"
-- Iteration 13 --
unicode(26) "\ttesting\ttesting	Ucwords"
-- Iteration 14 --
unicode(31) "TestingUcwords Testing Ucwords"
-- Iteration 15 --
unicode(32) "Testing\rucwords Testing Ucwords"
-- Iteration 16 --
unicode(34) "TestingUcwords  Testing 
Ucwords"
-- Iteration 17 --
unicode(36) "Testing\fucwords \f Testing 
Ucwords"
-- Iteration 18 --
unicode(35) "
Testing
Ucwords
 Testing 
 Ucwords"
-- Iteration 19 --
unicode(39) "\ntesting\nucwords\n Testing \n Ucwords"
-- Iteration 20 --
unicode(18) "UsingVerticalTab"
-- Iteration 21 --
unicode(20) "Using\vvertical\vtab"
-- Iteration 22 --
unicode(42) "T@@#$% %test ^test &test *test +test -test"
-- Iteration 23 --
unicode(40) "!test ~test `test` =test= @test@test.com"
-- Iteration 24 --
unicode(36) "/test/r	Est\ Ucwords	\y\y \yy\ /uu/"
-- Iteration 25 --
unicode(16) "!@#$%^&*()_+=-`~"
Done
