--TEST--
Test hebrevc() function : usage variations - test values for $max_chars_per_line argument
--FILE--
<?php

/* Prototype  : string hebrevc  ( string $hebrew_text  [, int $max_chars_per_line  ] )
 * Description: Convert logical Hebrew text to visual text
 * Source code: ext/standard/string.c
*/

echo "*** Testing hebrevc() function: with unexpected inputs for 'max_chars_per_line' argument ***\n";

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

// loop through with each element of the $texts array to test hebrevc() function
$count = 1;

$hebrew_text = "The hebrevcc function converts logical Hebrew text to visual text.\nThis function is similar to hebrevc() with the difference that it converts newlines (\n) to '<br>\n'.\nThe function tries to avoid breaking words.\n";

foreach($inputs as $max_chars_per_line) {
  echo "-- Iteration $count --\n";
  var_dump( hebrevc($hebrew_text, $max_chars_per_line) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing hebrevc() function: with unexpected inputs for 'max_chars_per_line' argument ***
-- Iteration 1 --
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 2 --
string(331) "xttel uaisv<br />
tot ext<br />
ewbrHel cagilos rtveonc<br />
ontincfuc vcreebh<br />
he.Ts<br />
neliewn<br />
tsernvcot i<br />
atthe ncrefeifd<br />
het<br />
thwi) c(evbrheo t<br />
arilims<br />
isn ioctunf<br />
isTh) (<br />
r <b'<br />
to<<br />
.'s<br />
rdwog inakreb<br />
idvoa<br />
tos ietrn ioctunf<br />
he.T<br />
"
-- Iteration 3 --
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 4 --
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 5 --
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 6 --
string(421) "txet<br />
lausiv<br />
ot<br />
txet<br />
werbeH<br />
lacigol<br />
strevnoc<br />
noitcnuf<br />
ccverbeh<br />
ehT.<br />
senilwen<br />
strevnoc<br />
ti<br />
taht<br />
ecnereffid<br />
eht<br />
htiw<br />
)(cverbeh<br />
ot<br />
ralimis<br />
si<br />
noitcnuf<br />
sihT<br />
)<br />
(<br />
rb<'<br />
ot<<br />
'.<br />
sdrow<br />
gnikaerb<br />
diova<br />
ot<br />
seirt<br />
noitcnuf<br />
ehT.<br />
"
-- Iteration 7 --
string(373) "text<br />
to visual<br />
text<br />
Hebrew<br />
logical<br />
converts<br />
function<br />
hebrevcc<br />
.The<br />
newlines<br />
converts<br />
that it<br />
difference<br />
with the<br />
hebrevc()<br />
similar to<br />
is<br />
function<br />
) This<br />
<to '<br (<br />
.'<br />
words<br />
breaking<br />
to avoid<br />
tries<br />
function<br />
.The<br />
"
-- Iteration 8 --
string(421) "txet<br />
lausiv<br />
ot<br />
txet<br />
werbeH<br />
lacigol<br />
strevnoc<br />
noitcnuf<br />
ccverbeh<br />
ehT.<br />
senilwen<br />
strevnoc<br />
ti<br />
taht<br />
ecnereffid<br />
eht<br />
htiw<br />
)(cverbeh<br />
ot<br />
ralimis<br />
si<br />
noitcnuf<br />
sihT<br />
)<br />
(<br />
rb<'<br />
ot<<br />
'.<br />
sdrow<br />
gnikaerb<br />
diova<br />
ot<br />
seirt<br />
noitcnuf<br />
ehT.<br />
"
-- Iteration 9 --
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 10 --

Warning: hebrevc() expects parameter 2 to be integer, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: hebrevc() expects parameter 2 to be integer, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: hebrevc() expects parameter 2 to be integer, array given in %s on line %d
NULL
-- Iteration 13 --
string(331) "xttel uaisv<br />
tot ext<br />
ewbrHel cagilos rtveonc<br />
ontincfuc vcreebh<br />
he.Ts<br />
neliewn<br />
tsernvcot i<br />
atthe ncrefeifd<br />
het<br />
thwi) c(evbrheo t<br />
arilims<br />
isn ioctunf<br />
isTh) (<br />
r <b'<br />
to<<br />
.'s<br />
rdwog inakreb<br />
idvoa<br />
tos ietrn ioctunf<br />
he.T<br />
"
-- Iteration 14 --
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 15 --
string(331) "xttel uaisv<br />
tot ext<br />
ewbrHel cagilos rtveonc<br />
ontincfuc vcreebh<br />
he.Ts<br />
neliewn<br />
tsernvcot i<br />
atthe ncrefeifd<br />
het<br />
thwi) c(evbrheo t<br />
arilims<br />
isn ioctunf<br />
isTh) (<br />
r <b'<br />
to<<br />
.'s<br />
rdwog inakreb<br />
idvoa<br />
tos ietrn ioctunf<br />
he.T<br />
"
-- Iteration 16 --
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 17 --
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 18 --
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 19 --

Warning: hebrevc() expects parameter 2 to be integer, string given in %s on line %d
NULL
-- Iteration 20 --

Warning: hebrevc() expects parameter 2 to be integer, string given in %s on line %d
NULL
-- Iteration 21 --

Notice: A non well formed numeric value encountered in %s on line %d
string(349) "textual vis<br />
to<br />
textrew Heb<br />
icallog<br />
ertsconvion unctf<br />
evcchebrThe .<br />
inesnewlrts onvec<br />
it<br />
thatnce feredif<br />
the<br />
withc() brevhe<br />
to<br />
ilarsim<br />
is<br />
tionfunchis ) T<br />
(<br />
'<br<to .'<br />
<br />
ordsw<br />
kingbreaoid av<br />
to<br />
riest<br />
tionfuncThe .<br />
"
-- Iteration 22 --

Notice: A non well formed numeric value encountered in %s on line %d
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 23 --

Notice: A non well formed numeric value encountered in %s on line %d
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 24 --

Warning: hebrevc() expects parameter 2 to be integer, object given in %s on line %d
NULL
-- Iteration 25 --

Warning: hebrevc() expects parameter 2 to be integer, resource given in %s on line %d
NULL
-- Iteration 26 --
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
-- Iteration 27 --
string(241) ".The hebrevcc function converts logical Hebrew text to visual text<br />
) This function is similar to hebrevc() with the difference that it converts newlines<br />
<to '<br (<br />
.'<br />
.The function tries to avoid breaking words<br />
"
===DONE===
