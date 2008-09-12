--TEST--
TextIterator::getAll(): Simple test
--FILE--
<?php

$x = new textiterator('asda');
var_dump($x->getAll());

?>
--EXPECT--
array(4) {
  [0]=>
  unicode(1) "a"
  [1]=>
  unicode(1) "s"
  [2]=>
  unicode(1) "d"
  [3]=>
  unicode(1) "a"
}
