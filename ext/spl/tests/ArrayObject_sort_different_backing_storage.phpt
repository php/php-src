--TEST--
Test sorting of various ArrayObject backing storage
--FILE--
<?php

$obj = (object)['a' => 2, 'b' => 1];
$ao = new ArrayObject($obj);
$ao->uasort(function($a, $b) { return $a <=> $b; });
var_dump($ao);

$ao2 = new ArrayObject($ao);
$ao2->uasort(function($a, $b) { return $b <=> $a; });
var_dump($ao2);

$ao3 = new ArrayObject();
$ao3->exchangeArray($ao3);
$ao3->a = 2;
$ao3->b = 1;
$ao3->uasort(function($a, $b) { return $a <=> $b; });
var_dump($ao3);

$ao4 = new ArrayObject([]);
$ao4->uasort(function($a, $b) { return $a <=> $b; });
var_dump($ao4);

$ao5 = new ArrayObject(['a' => 2, 'b' => 1]);
$ao5->uasort(function($a, $b) { return $a <=> $b; });
var_dump($ao5);

?>
--EXPECT--
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(stdClass)#1 (2) {
    ["b"]=>
    int(1)
    ["a"]=>
    int(2)
  }
}
object(ArrayObject)#3 (1) {
  ["storage":"ArrayObject":private]=>
  object(ArrayObject)#2 (1) {
    ["storage":"ArrayObject":private]=>
    object(stdClass)#1 (2) {
      ["a"]=>
      int(2)
      ["b"]=>
      int(1)
    }
  }
}
object(ArrayObject)#4 (2) {
  ["b"]=>
  int(1)
  ["a"]=>
  int(2)
}
object(ArrayObject)#5 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
object(ArrayObject)#6 (1) {
  ["storage":"ArrayObject":private]=>
  array(2) {
    ["b"]=>
    int(1)
    ["a"]=>
    int(2)
  }
}
