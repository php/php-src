--TEST--
Test array_merge() function : basic functionality 
--FILE--
<?php
/* Prototype  : array array_merge(array $arr1, array $arr2 [, array $...])
 * Description: Merges elements from passed arrays into one array 
 * Source code: ext/standard/array.c
 */

/*
 * Test basic functionality of array_merge()
 */

echo "*** Testing array_merge() : basic functionality ***\n";

//indexed array
$array1 = array ('zero', 'one', 'two');
//associative array
$array2 = array ('a' => 1, 'b' => 2, 'c' => 3);

var_dump(array_merge($array1, $array2));

var_dump(array_merge($array2, $array1));

echo "Done";
?>

--EXPECT--
*** Testing array_merge() : basic functionality ***
array(6) {
  [0]=>
  unicode(4) "zero"
  [1]=>
  unicode(3) "one"
  [2]=>
  unicode(3) "two"
  [u"a"]=>
  int(1)
  [u"b"]=>
  int(2)
  [u"c"]=>
  int(3)
}
array(6) {
  [u"a"]=>
  int(1)
  [u"b"]=>
  int(2)
  [u"c"]=>
  int(3)
  [0]=>
  unicode(4) "zero"
  [1]=>
  unicode(3) "one"
  [2]=>
  unicode(3) "two"
}
Done
