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
* chunk_split() : passing single quoted strings for 'str' to the function
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
  '     Testing with    Tab     ',  //string with TAB                         
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
--EXPECT--
*** Testing chunk_split() : with different single quoted 'str' ***
-- Iteration 0 --
unicode(3) "):("
-- Iteration 1 --
unicode(4) " ):("
-- Iteration 2 --
unicode(30) "This is):( simple):( string):("
-- Iteration 3 --
unicode(35) "It's st):(ring wi):(th quot):(es):("
-- Iteration 4 --
unicode(46) "This co):(ntains ):(@ # $ %):( ^ & ch):(ars):("
-- Iteration 5 --
unicode(59) "This st):(ring\tc):(ontains):(\rwhite):( space\):(nchars):("
-- Iteration 6 --
unicode(47) "This is):( string):( with 1):(234 num):(bers):("
-- Iteration 7 --
unicode(68) "This is):( string):( with \):(0 and "):(.chr(0)):(."null ):(chars):("
-- Iteration 8 --
unicode(74) "This is):( string):( with  ):(  multi):(ple    ):(     sp):(ace cha):(r):("
-- Iteration 9 --
unicode(46) "This is):( to che):(ck stri):(ng with):( ()):("
-- Iteration 10 --
unicode(44) "     Te):(sting w):(ith    ):(Tab    ):( ):("
-- Iteration 11 --
unicode(55) "Testing):( invali):(d \k an):(d \m es):(cape ch):(ar):("
-- Iteration 12 --
unicode(46) "This is):( to che):(ck with):( \n and):( \t):("
Done
