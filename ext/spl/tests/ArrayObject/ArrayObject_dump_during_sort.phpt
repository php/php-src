--TEST--
Dumping an ArrayObject while it is being sorted
--FILE--
<?php

$ao = new ArrayObject([1, 2, 3]);
$i = 0;
$ao->uasort(function($a, $b) use ($ao, &$i) {
    if ($i++ == 0) {
        var_dump($ao);
    }
    return $a <=> $b;
});

?>
--EXPECT--
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
