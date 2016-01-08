--TEST--
Test array_binary_search() function : usage variations - different needdle values
--FILE--
<?php
/*
 * Prototype  : bool array_binary_search ( mixed $needle, array $haystack [, bool $strict] )
 * Description: Used binary search to search haystack for needle and returns TRUE  
 *              if it is found in the array, FALSE otherwise.
 * Source Code: ext/standard/array.c
*/

/* Test array_binary_search with different possible needle values */

echo "*** Testing array_binary_search with different needle values ***\n";
$array1 = array(1,2,3,4,5,6,7,8,9,10);

$array2 = array(1,"2",3,"4","5",6,"7",8,"9",10);

$array3 = array(1,"two",3,"four",5,"six",7,"eight",9,"ten");

$array4 = array("and","ball","decimal","message","ontario","person","test","version","zone");

$array5 = array(1,"1",2,"2",3,"3",4,"4",5,"5");

sort($array1);
//strict option OFF
var_dump(array_binary_search(4,$array1));  
//strict option ON
var_dump(array_binary_search(4,$array1,TRUE));  
//strict option OFF
var_dump(array_binary_search(4,$array1,FALSE));  
		
//strict option OFF
var_dump(array_binary_search(6,$array1));       
//strict option ON
var_dump(array_binary_search(6,$array1,TRUE));       
//strict option OFF
var_dump(array_binary_search(6,$array1,FALSE));  

var_dump(array_binary_search("6",$array1));
//strict option ON
var_dump(array_binary_search("6",$array1,TRUE));
//strict option OFF
var_dump(array_binary_search("6",$array1,FALSE));

var_dump(array_binary_search("16",$array1));
//strict option ON
var_dump(array_binary_search("16",$array1,TRUE));
//strict option OFF
var_dump(array_binary_search("16",$array1,FALSE));

sort($array2);

var_dump(array_binary_search(1,$array2));
//strict option ON
var_dump(array_binary_search(1,$array2,TRUE));
//strict option OFF
var_dump(array_binary_search(1,$array2,FALSE));

var_dump(array_binary_search(10,$array2));
//strict option ON
var_dump(array_binary_search(10,$array2,TRUE));
//strict option OFF
var_dump(array_binary_search(10,$array2,FALSE));

var_dump(array_binary_search("10",$array2));
//strict option ON
var_dump(array_binary_search("10",$array2,TRUE));
//strict option OFF
var_dump(array_binary_search("10",$array2,FALSE));

var_dump(array_binary_search(5,$array2));
//strict option ON
var_dump(array_binary_search(5,$array2,TRUE));
//strict option OFF
var_dump(array_binary_search(5,$array2,FALSE));

sort($array3);

var_dump(array_binary_search("six",$array3));
//strict option ON
var_dump(array_binary_search("six",$array3,TRUE));
//strict option OFF
var_dump(array_binary_search("six",$array3,FALSE));

var_dump(array_binary_search(9.00,$array3));
//strict option ON
var_dump(array_binary_search(9.00,$array3,TRUE));
//strict option OFF
var_dump(array_binary_search(9.00,$array3,FALSE));

var_dump(array_binary_search("9.00",$array3));
//strict option ON
var_dump(array_binary_search("9.00",$array3,TRUE));
//strict option OFF
var_dump(array_binary_search("9.00",$array3,FALSE));

var_dump(array_binary_search(7,$array3));
//strict option ON
var_dump(array_binary_search(7,$array3,TRUE));
//strict option OFF
var_dump(array_binary_search(7,$array3,FALSE));

sort($array4);

var_dump(array_binary_search('ontario',$array4));
//strict option ON
var_dump(array_binary_search('ontario',$array4,TRUE));
//strict option OFF
var_dump(array_binary_search('ontario',$array4,FALSE));

var_dump(array_binary_search('ONTARIO',$array4));
//strict option ON
var_dump(array_binary_search('ONTARIO',$array4,TRUE));
//strict option OFF
var_dump(array_binary_search('ONTARIO',$array4,FALSE));

var_dump(array_binary_search('zOne',$array4));
//strict option ON
var_dump(array_binary_search('zOne',$array4,TRUE));
//strict option OFF
var_dump(array_binary_search('zOne',$array4,FALSE));

var_dump(array_binary_search('sadas',$array4));
//strict option ON
var_dump(array_binary_search('sadas',$array4,TRUE));
//strict option OFF
var_dump(array_binary_search('sadas',$array4,FALSE));

sort($array5);

var_dump(array_binary_search(3,$array5));
//strict option ON
var_dump(array_binary_search(3,$array5,TRUE));
//strict option OFF
var_dump(array_binary_search(3,$array5,FALSE));

var_dump(array_binary_search("3",$array5));
//strict option ON
var_dump(array_binary_search("3",$array5,TRUE));
//strict option OFF
var_dump(array_binary_search("3",$array5,FALSE));

var_dump(array_binary_search(3.20,$array5));
//strict option ON
var_dump(array_binary_search(3.20,$array5,TRUE));
//strict option OFF
var_dump(array_binary_search(3.20,$array5,FALSE));

var_dump(array_binary_search(33,$array5));
//strict option ON
var_dump(array_binary_search(33,$array5,TRUE));
//strict option OFF
var_dump(array_binary_search(33,$array5,FALSE));
echo "Done\n";
?>
--EXPECTF--
*** Testing array_binary_search with different needle values ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Done

