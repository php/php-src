--TEST--
SPL: RegexIterator::GET_MATCH
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

class MyRegexIterator extends RegexIterator
{
	function show()
	{
		foreach($this as $k => $v)
		{
			var_dump($k);
			var_dump($v);
		}
	}
}

$ar = new ArrayIterator(array('1','1,2','1,2,3','',NULL,array(),'FooBar',',',',,'));
$it = new MyRegexIterator($ar, '/(\d),(\d)/', RegexIterator::GET_MATCH);
$it->show();

$it = new MyRegexIterator($ar, '/(\d)/', RegexIterator::GET_MATCH);
$it->show();

var_dump($ar);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
int(1)
array(3) {
  [0]=>
  string(3) "1,2"
  [1]=>
  string(1) "1"
  [2]=>
  string(1) "2"
}
int(2)
array(3) {
  [0]=>
  string(3) "1,2"
  [1]=>
  string(1) "1"
  [2]=>
  string(1) "2"
}
int(0)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
int(1)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
int(2)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
object(ArrayIterator)#%d (9) {
  [0]=>
  %s(1) "1"
  [1]=>
  %s(3) "1,2"
  [2]=>
  %s(5) "1,2,3"
  [3]=>
  %s(0) ""
  [4]=>
  NULL
  [5]=>
  array(0) {
  }
  [6]=>
  %s(6) "FooBar"
  [7]=>
  %s(1) ","
  [8]=>
  %s(2) ",,"
}
===DONE===
