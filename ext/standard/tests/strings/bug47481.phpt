--TEST--
Bug #47481 (natcasesort() does not sort extended ASCII characters correctly)
--FILE--
<?php
/*
 * Test natcasesort() with extended ASCII characters
 */

$array = array ('Süden', 'spielen','Sonne','Wind','Regen','Meer');
echo "\n-- Before sorting: --\n";
var_dump($array);

echo "\n-- After Sorting: --\n";
var_dump(natcasesort($array));
var_dump($array);

echo "Done";
?>
--EXPECT--
-- Before sorting: --
array(6) {
  [0]=>
  string(6) "Süden"
  [1]=>
  string(7) "spielen"
  [2]=>
  string(5) "Sonne"
  [3]=>
  string(4) "Wind"
  [4]=>
  string(5) "Regen"
  [5]=>
  string(4) "Meer"
}

-- After Sorting: --
bool(true)
array(6) {
  [5]=>
  string(4) "Meer"
  [4]=>
  string(5) "Regen"
  [2]=>
  string(5) "Sonne"
  [1]=>
  string(7) "spielen"
  [0]=>
  string(6) "Süden"
  [3]=>
  string(4) "Wind"
}
Done
