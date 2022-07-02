--TEST--
Test compact() function : usage variations  - arrays containing references.
--FILE--
<?php
/*
* compact variations - arrays with references
*/
echo "*** Testing compact() : usage variations  - arrays containing references ***\n";
$a = 1;
$b = 2;
$c = 3;
$string = "c";
$arr1 = array("a", &$arr1);
$arr2 = array("a", array(array(array("b"))));
$arr2[1][0][0][] = &$arr2;
$arr2[1][0][0][] = &$arr2[1];
$arr3 = array(&$string);

try {
    var_dump(compact($arr1));
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(compact($arr2));
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump(compact($arr3));

?>
--EXPECT--
*** Testing compact() : usage variations  - arrays containing references ***
Recursion detected
Recursion detected
array(1) {
  ["c"]=>
  int(3)
}
