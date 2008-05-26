--TEST--
Bug #22088 (array_shift() leaves next index to be +1 too much)
--FILE--
<?php

$a = array('a', 'b', 'c');
$last = array_shift ($a);
$a[] = 'a';
var_dump($a);

$a = array('a' => 1, 'b' => 2, 'c' => 3);
$last = array_shift ($a);
$a[] = 'a';
var_dump($a);

?>
--EXPECT--
array(3) {
  [0]=>
  unicode(1) "b"
  [1]=>
  unicode(1) "c"
  [2]=>
  unicode(1) "a"
}
array(3) {
  [u"b"]=>
  int(2)
  [u"c"]=>
  int(3)
  [0]=>
  unicode(1) "a"
}
