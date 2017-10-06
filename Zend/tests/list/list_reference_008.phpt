--TEST--
"Reference Unpacking - Oddities" list()
--FILE--
<?php
$a = 1;
$b =& $a;
$arr = [&$a, &$b];
list(&$a, &$b) = $arr;
var_dump($a, $b, $arr);
$b++;
var_dump($a, $b, $arr);
unset($a, $b, $arr);

/*
 * $a is first set as a reference to the 0'th elem, '1'
 * $a is then set to the value of the 1'st elem, '2'
 * $arr would look like, [2,2]
 * Increment $a, and it should be [3, 2]
 */
$arr = [1, 2];
list(&$a, $a) = $arr;
var_dump($a);
$a++;
var_dump($arr);
unset($a, $arr);

/*
 * We do not allow references to the same variable of rhs.
 */
$a = [1, 2];
list(&$a, $a) = $a;
var_dump($a);
?>
--EXPECTF--
int(1)
int(1)
array(2) {
  [0]=>
  &int(1)
  [1]=>
  &int(1)
}
int(2)
int(2)
array(2) {
  [0]=>
  &int(2)
  [1]=>
  &int(2)
}
int(2)
array(2) {
  [0]=>
  &int(3)
  [1]=>
  int(2)
}

Notice: Attempting to set reference to non referenceable value in %s on line %d
int(2)
