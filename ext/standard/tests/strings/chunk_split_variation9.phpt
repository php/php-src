--TEST--
Test chunk_split() function : usage variations - different double quoted strings for 'ending' argument
--FILE--
<?php
/*
* passing different double quoted strings for 'ending' argument to chunk_split()
* here 'chunklen' is set to 6.5
*/

echo "*** Testing chunk_split() : different strings for 'ending' ***\n";

//Initializing variables
$str = "This is to test chunk_split() with various ending string";
$chunklen = 6;

//different values for 'ending' argument
$values = array (
  "",  //empty
  " ",  //space
  "a",  //Single char
  "ENDING",  //regular string
  "@#$%^",  //Special chars

  // white space chars
  "\t",
  "\n",
  "\r",
  "\r\n",

  "\0",  //Null char
  "123",  //Numeric
  "(MSG)",  //With ( and )
  ")ending string(",  //sentence as ending string
  ")numbers 1234(",
  ")speci@! ch@r$("
);

//loop through element of values for 'ending'
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration $count --\n";
  var_dump( chunk_split($str, $chunklen, $values[$count]) );
}

echo "Done"
?>
--EXPECTF--
*** Testing chunk_split() : different strings for 'ending' ***
-- Iteration 0 --
string(56) "This is to test chunk_split() with various ending string"
-- Iteration 1 --
string(66) "This i s to t est ch unk_sp lit()  with v arious  endin g stri ng "
-- Iteration 2 --
string(66) "This ias to taest chaunk_spalit() awith vaariousa endinag strianga"
-- Iteration 3 --
string(116) "This iENDINGs to tENDINGest chENDINGunk_spENDINGlit() ENDINGwith vENDINGariousENDING endinENDINGg striENDINGngENDING"
-- Iteration 4 --
string(106) "This i@#$%^s to t@#$%^est ch@#$%^unk_sp@#$%^lit() @#$%^with v@#$%^arious@#$%^ endin@#$%^g stri@#$%^ng@#$%^"
-- Iteration 5 --
string(66) "This i	s to t	est ch	unk_sp	lit() 	with v	arious	 endin	g stri	ng	"
-- Iteration 6 --
string(66) "This i
s to t
est ch
unk_sp
lit() 
with v
arious
 endin
g stri
ng
"
-- Iteration 7 --
string(66) "This is to test chunk_split() with various ending string"
-- Iteration 8 --
string(76) "This i
s to t
est ch
unk_sp
lit() 
with v
arious
 endin
g stri
ng
"
-- Iteration 9 --
string(66) "This i%0s to t%0est ch%0unk_sp%0lit() %0with v%0arious%0 endin%0g stri%0ng%0"
-- Iteration 10 --
string(86) "This i123s to t123est ch123unk_sp123lit() 123with v123arious123 endin123g stri123ng123"
-- Iteration 11 --
string(106) "This i(MSG)s to t(MSG)est ch(MSG)unk_sp(MSG)lit() (MSG)with v(MSG)arious(MSG) endin(MSG)g stri(MSG)ng(MSG)"
-- Iteration 12 --
string(206) "This i)ending string(s to t)ending string(est ch)ending string(unk_sp)ending string(lit() )ending string(with v)ending string(arious)ending string( endin)ending string(g stri)ending string(ng)ending string("
-- Iteration 13 --
string(196) "This i)numbers 1234(s to t)numbers 1234(est ch)numbers 1234(unk_sp)numbers 1234(lit() )numbers 1234(with v)numbers 1234(arious)numbers 1234( endin)numbers 1234(g stri)numbers 1234(ng)numbers 1234("
-- Iteration 14 --
string(206) "This i)speci@! ch@r$(s to t)speci@! ch@r$(est ch)speci@! ch@r$(unk_sp)speci@! ch@r$(lit() )speci@! ch@r$(with v)speci@! ch@r$(arious)speci@! ch@r$( endin)speci@! ch@r$(g stri)speci@! ch@r$(ng)speci@! ch@r$("
Done
