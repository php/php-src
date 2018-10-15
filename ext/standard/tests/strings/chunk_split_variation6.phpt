--TEST--
Test chunk_split() function : usage variations - single quoted strings for 'str' argument
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* passing different single quoted strings as 'str' argument to the function
* 'chunklen' is set to 7 and 'ending' is '):('
*/

echo "*** Testing chunk_split() : with different single quoted 'str' ***\n";

//Initializing variables
$chunklen = 7;
$ending = "):(";

//different single quoted string for 'str'
$values = array(
  '',  //empty
  ' ',  //space
  'This is simple string',  //regular string
  'It\'s string with quotes',
  'This contains @ # $ % ^ & chars',   //special characters
  'This string\tcontains\rwhite space\nchars',  //with white space chars
  'This is string with 1234 numbers',
  'This is string with \0 and ".chr(0)."null chars',  //for binary safe
  'This is string with    multiple         space char',
  'This is to check string with ()',
  '     Testing with    multiple spaces     ',
  'Testing invalid \k and \m escape char',
  'This is to check with \\n and \\t'
);


//Loop through each element of values for 'str'
for($count = 0;$count < count($values);$count++) {
  echo "-- Iteration $count --\n";
  var_dump( chunk_split($values[$count], $chunklen, $ending) );
}

echo "Done"
?>
--EXPECTF--
*** Testing chunk_split() : with different single quoted 'str' ***
-- Iteration 0 --
string(3) "):("
-- Iteration 1 --
string(4) " ):("
-- Iteration 2 --
string(30) "This is):( simple):( string):("
-- Iteration 3 --
string(35) "It's st):(ring wi):(th quot):(es):("
-- Iteration 4 --
string(46) "This co):(ntains ):(@ # $ %):( ^ & ch):(ars):("
-- Iteration 5 --
string(59) "This st):(ring\tc):(ontains):(\rwhite):( space\):(nchars):("
-- Iteration 6 --
string(47) "This is):( string):( with 1):(234 num):(bers):("
-- Iteration 7 --
string(68) "This is):( string):( with \):(0 and "):(.chr(0)):(."null ):(chars):("
-- Iteration 8 --
string(74) "This is):( string):( with  ):(  multi):(ple    ):(     sp):(ace cha):(r):("
-- Iteration 9 --
string(46) "This is):( to che):(ck stri):(ng with):( ()):("
-- Iteration 10 --
string(59) "     Te):(sting w):(ith    ):(multipl):(e space):(s     ):("
-- Iteration 11 --
string(55) "Testing):( invali):(d \k an):(d \m es):(cape ch):(ar):("
-- Iteration 12 --
string(46) "This is):( to che):(ck with):( \n and):( \t):("
Done
