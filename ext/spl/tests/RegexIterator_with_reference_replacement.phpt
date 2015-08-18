--TEST--
RegexIterator with $replacement being a reference
--FILE--
<?php
$a = new ArrayIterator(array('test1', 'test2', 'test3'));
$i = new RegexIterator($a, '/^(test)(\d+)/', RegexIterator::REPLACE);
$r = '$2:$1';
$i->replacement =& $r;
var_dump(iterator_to_array($i));
?>
--EXPECT--
array(3) {
  [0]=>
  string(6) "1:test"
  [1]=>
  string(6) "2:test"
  [2]=>
  string(6) "3:test"
}
