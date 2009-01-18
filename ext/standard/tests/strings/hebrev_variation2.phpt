--TEST--
Test hebrev() function : usage variations - test values for $max_chars_per_line argument
--FILE--
<?php

/* Prototype  : string hebrev  ( string $hebrew_text  [, int $max_chars_per_line  ] )
 * Description: Convert logical Hebrew text to visual text
 * Source code: ext/standard/string.c
*/

echo "*** Testing hebrev() function: with unexpected inputs for 'max_chars_per_line' argument ***\n";

//get an unset variable
$unset_var = 'string_val';
unset($unset_var);

//defining a class
class sample  {
  public function __toString() {
    return "sample object";
  } 
}

//getting the resource
$file_handle = fopen(__FILE__, "r");

// array with different values for $max_chars_per_line
$inputs =  array (

		  // integer values
/*1*/	  0,
		  1,
		  255,
		  256,
	      2147483647,
		  -2147483648,
		
		  // float values
/*7*/	  10.5,
		  -20.5,
		  10.1234567e5,
		
		  // array values
/*10*/	  array(),
		  array(0),
		  array(1, 2),
		
		  // boolean values
/*13*/	  true,
		  false,
		  TRUE,
		  FALSE,
		
		  // null values
/*17*/	  NULL,
		  null,
		  
		  // string values
/*19*/	  "abc",
		  'abc',
		  "3abc",
		  "0abc",
		  "0x3",
		
		  // objects
/*24*/	  new sample(),
		
		  // resource
/*25*/	  $file_handle,
		
		  // undefined variable
/*26*/	  @$undefined_var,
		
		  // unset variable
/*27*/	  @$unset_var
);

// loop through with each element of the $texts array to test hebrev() function
$count = 1;

$hebrew_text = "The hebrev function converts logical Hebrew text to visual text.\nThe function tries to avoid breaking words.\n";

foreach($inputs as $max_chars_per_line) {
  echo "-- Iteration $count --\n";
  var_dump( hebrev($hebrew_text, $max_chars_per_line) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing hebrev() function: with unexpected inputs for 'max_chars_per_line' argument ***
-- Iteration 1 --
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
-- Iteration 2 --
string(109) "xttel uaisv
tot ext
ewbrHel cagilos rtveonc
ontincfuv reebh
he.Ts
rdwog inakreb
idvoa
tos ietrn ioctunf
he.T
"
-- Iteration 3 --
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
-- Iteration 4 --
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
-- Iteration 5 --
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
-- Iteration 6 --
string(109) "txet
lausiv
ot
txet
werbeH
lacigol
strevnoc
noitcnuf
verbeh
ehT.
sdrow
gnikaerb
diova
ot
seirt
noitcnuf
ehT.
"
-- Iteration 7 --
string(109) "text
to visual
text
Hebrew
logical
converts
function
hebrev
.The
words
breaking
to avoid
tries
function
.The
"
-- Iteration 8 --
string(109) "txet
lausiv
ot
txet
werbeH
lacigol
strevnoc
noitcnuf
verbeh
ehT.
sdrow
gnikaerb
diova
ot
seirt
noitcnuf
ehT.
"
-- Iteration 9 --
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
-- Iteration 10 --

Warning: hebrev() expects parameter 2 to be long, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: hebrev() expects parameter 2 to be long, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: hebrev() expects parameter 2 to be long, array given in %s on line %d
NULL
-- Iteration 13 --
string(109) "xttel uaisv
tot ext
ewbrHel cagilos rtveonc
ontincfuv reebh
he.Ts
rdwog inakreb
idvoa
tos ietrn ioctunf
he.T
"
-- Iteration 14 --
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
-- Iteration 15 --
string(109) "xttel uaisv
tot ext
ewbrHel cagilos rtveonc
ontincfuv reebh
he.Ts
rdwog inakreb
idvoa
tos ietrn ioctunf
he.T
"
-- Iteration 16 --
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
-- Iteration 17 --
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
-- Iteration 18 --
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
-- Iteration 19 --

Warning: hebrev() expects parameter 2 to be long, string given in %s on line %d
NULL
-- Iteration 20 --

Warning: hebrev() expects parameter 2 to be long, string given in %s on line %d
NULL
-- Iteration 21 --

Notice: A non well formed numeric value encountered in %s on line %d
string(109) "textual vis
to
textrew Heb
icallog
ertsconvion unctf
brevhe
.Therds
wo
kingbreaoid av
to
riest
tionfuncThe .
"
-- Iteration 22 --

Notice: A non well formed numeric value encountered in %s on line %d
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
-- Iteration 23 --
string(109) "textual vis
to
textrew Heb
icallog
ertsconvion unctf
brevhe
.Therds
wo
kingbreaoid av
to
riest
tionfuncThe .
"
-- Iteration 24 --

Warning: hebrev() expects parameter 2 to be long, object given in %s on line %d
NULL
-- Iteration 25 --

Warning: hebrev() expects parameter 2 to be long, resource given in %s on line %d
NULL
-- Iteration 26 --
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
-- Iteration 27 --
string(109) ".The hebrev function converts logical Hebrew text to visual text
.The function tries to avoid breaking words
"
===DONE===