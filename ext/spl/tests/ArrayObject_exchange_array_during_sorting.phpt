--TEST--
Can't use exchangeArray() while ArrayObject is being sorted
--FILE--
<?php

$ao = new ArrayObject([1, 2, 3]);
$i = 0;
$ao->uasort(function($a, $b) use ($ao, &$i) {
    if ($i++ == 0) {
        $ao->exchangeArray([4, 5, 6]);
        var_dump($ao);
    }
    return $a <=> $b;
});

?>
--EXPECTF--
Warning: Modification of ArrayObject during sorting is prohibited in %s on line %d
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
