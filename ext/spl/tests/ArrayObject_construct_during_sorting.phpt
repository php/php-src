--TEST--
Can't use __construct() to replace the backing store while ArrayObject is being sorted
--FILE--
<?php

$ao = new ArrayObject([1, 2, 3]);
$other = new ArrayObject([4, 5, 6]);
$i = 0;
$ao->uasort(function($a, $b) use ($ao, $other, &$i) {
    if ($i++ == 0) {
        try {
            $ao->__construct($other);
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
    return $a <=> $b;
});
var_dump($ao);

?>
--EXPECT--
Modification of ArrayObject during sorting is prohibited
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
