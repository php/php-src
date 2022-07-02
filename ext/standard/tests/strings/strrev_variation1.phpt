--TEST--
Test strrev() function : usage variations - double quoted strings
--FILE--
<?php
/* Testing strrev() function with various double quoted strings for 'str' */

echo "*** Testing strrev() : with various double quoted strings ***\n";
$str = "Hiiii";

$strings = array(
  //strings containing escape chars
  "hello\\world",
  "hello\$world",
  "\ttesting\ttesting\tstrrev",
  "testing\rstrrev testing strrev",
  "testing\fstrrev \f testing \nstrrev",
  "\ntesting\nstrrev\n testing \n strrev",
  "using\vvertical\vtab",

  //polyndrome strings
  "HelloolleH",
  "ababababababa",

  //numeric + strings
  "Hello123",
  "123Hello",
  "123.34Hello",
  "Hello1.234",
  "123Hello1.234",

  //concatenated strings
  "Hello".chr(0)."World",
  "Hello"."world",
  "Hello".$str,

  //strings containing white spaces
  "              h",
  "h              ",
  "      h        ",
  "h  e  l  l  o  ",

  //strings containing quotes
  "\hello\'world",
  "hello\"world",

  //special chars in strings
  "t@@#$% %test ^test &test *test +test -test",
  "!test ~test `test` =test= @test@test.com",
  "/test/r\test\strrev\t\\u /uu/",

  //only special chars
  "!@#$%^&*()_+=-`~"
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
*** Testing strrev() : with various double quoted strings ***

-- Iteration 1 --
string(11) "dlrow\olleh"

-- Iteration 2 --
string(11) "dlrow$olleh"

-- Iteration 3 --
string(23) "verrts	gnitset	gnitset	"

-- Iteration 4 --
string(29) "verrts gnitset verrtsgnitset"

-- Iteration 5 --
string(32) "verrts
 gnitset  verrtsgnitset"

-- Iteration 6 --
string(33) "verrts 
 gnitset 
verrts
gnitset
"

-- Iteration 7 --
string(18) "batlacitrevgnisu"

-- Iteration 8 --
string(10) "HelloolleH"

-- Iteration 9 --
string(13) "ababababababa"

-- Iteration 10 --
string(8) "321olleH"

-- Iteration 11 --
string(8) "olleH321"

-- Iteration 12 --
string(11) "olleH43.321"

-- Iteration 13 --
string(10) "432.1olleH"

-- Iteration 14 --
string(13) "432.1olleH321"

-- Iteration 15 --
string(11) "dlroW%0olleH"

-- Iteration 16 --
string(10) "dlrowolleH"

-- Iteration 17 --
string(10) "iiiiHolleH"

-- Iteration 18 --
string(15) "h              "

-- Iteration 19 --
string(15) "              h"

-- Iteration 20 --
string(15) "        h      "

-- Iteration 21 --
string(15) "  o  l  l  e  h"

-- Iteration 22 --
string(13) "dlrow'\olleh\"

-- Iteration 23 --
string(11) "dlrow"olleh"

-- Iteration 24 --
string(42) "tset- tset+ tset* tset& tset^ tset% %$#@@t"

-- Iteration 25 --
string(40) "moc.tset@tset@ =tset= `tset` tset~ tset!"

-- Iteration 26 --
string(26) "/uu/ u\	verrts\tse	r/tset/"

-- Iteration 27 --
string(16) "~`-=+_)(*&^%$#@!"
*** Done ***
