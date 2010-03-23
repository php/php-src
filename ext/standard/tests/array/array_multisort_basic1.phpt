--TEST--
Test array_multisort() function : basic functionality 
--FILE--
<?php
/* Prototype  : bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING]], ...])
 * Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_multisort() : basic functionality ***\n";

// Initialise all required variables
$ar1 = array("row1" => 2, "row2" => 1, "row3" => 1);
$ar2 = array("row1" => 2, "row2" => "aa", "row3" => "1");

echo "\n-- Testing array_multisort() function with all normal arguments --\n";
var_dump( array_multisort($ar1, SORT_ASC, SORT_REGULAR, $ar2, SORT_DESC, SORT_STRING) );
var_dump($ar1, $ar2);

echo "\n-- Testing array_multisort() function with one argument --\n";
var_dump( array_multisort($ar2) );
var_dump($ar2);


?>
===DONE===
--EXPECTF--
*** Testing array_multisort() : basic functionality ***

-- Testing array_multisort() function with all normal arguments --
bool(true)
array(3) {
  ["row2"]=>
  int(1)
  ["row3"]=>
  int(1)
  ["row1"]=>
  int(2)
}
array(3) {
  ["row2"]=>
  string(2) "aa"
  ["row3"]=>
  string(1) "1"
  ["row1"]=>
  int(2)
}

-- Testing array_multisort() function with one argument --
bool(true)
array(3) {
  ["row3"]=>
  string(1) "1"
  ["row2"]=>
  string(2) "aa"
  ["row1"]=>
  int(2)
}
===DONE===