--TEST--
Test chunk_split() function : basic functionality 
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line 
 * Source code: ext/standard/string.c
 * Alias to functions: 
*/

/*
* Testing chunk_split() for basic functionality by passing all possible 
* arguments as well as with default arguments chunklen and ending  
*/

echo "*** Testing chunk_split() : basic functionality ***\n";


// Initialise all required variables
$str = 'Testing';
$chunklen = 2;
$ending = '##';

// Calling chunk_split() with all possible arguments
echo "-- Testing chunk_split() with all possible arguments --\n"; 
var_dump( chunk_split($str, $chunklen, $ending) );


// Calling chunk_split() with default ending string
echo "-- Testing chunk_split() with default ending string --\n";
var_dump( chunk_split($str, $chunklen) );


//Calling chunk_split() with default chunklen and ending string
echo "-- Testing chunk_split() with default chunklen and ending string --\n";
var_dump( chunk_split($str) );
	
echo "Done"   
?>
--EXPECTF--
*** Testing chunk_split() : basic functionality ***
-- Testing chunk_split() with all possible arguments --
string(15) "Te##st##in##g##"
-- Testing chunk_split() with default ending string --
string(15) "Te
st
in
g
"
-- Testing chunk_split() with default chunklen and ending string --
string(9) "Testing
"
Done
