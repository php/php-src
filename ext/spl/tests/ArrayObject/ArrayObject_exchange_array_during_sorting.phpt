--TEST--
Can't use exchangeArray() while ArrayObject is being sorted
--FILE--
<?php

$ao = new ArrayObject([1, 2, 3]);
$i = 0;
$ao->uasort(function($a, $b) use ($ao, &$i) {
    if ($i++ == 0) {
        try {
            $ao->exchangeArray([4, 5, 6]);
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        var_dump($ao);
    }
    return $a <=> $b;
});

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
