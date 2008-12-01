--TEST--
Test array_multisort() function : basic functionality 
--FILE--
<?php
/* Prototype  : bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING]], ...])
 * Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_multisort() : basic functionality - renumbering of numeric keys ***\n";

// Initialise all required variables
$ar1 = array( "strkey" => 2,  1,  9 => 1);
$ar2 = array( 2, "aa" , "1");

echo "\n-- Testing array_multisort() function with all normal arguments --\n";
var_dump( array_multisort($ar1, SORT_ASC, SORT_REGULAR, $ar2, SORT_ASC, SORT_NUMERIC) );
var_dump($ar1, $ar2);

?>
===DONE===
--EXPECTF--
*** Testing array_multisort() : basic functionality - renumbering of numeric keys ***

-- Testing array_multisort() function with all normal arguments --
bool(true)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  ["strkey"]=>
  int(2)
}
array(3) {
  [0]=>
  string(2) "aa"
  [1]=>
  string(1) "1"
  [2]=>
  int(2)
}
===DONE===