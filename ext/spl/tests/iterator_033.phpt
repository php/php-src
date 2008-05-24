--TEST--
SPL: ParentIterator
--FILE--
<?php

$it = new ParentIterator(new RecursiveArrayIterator(array(1,array(21,22, array(231)),3)));

foreach(new RecursiveIteratorIterator($it) as $k=>$v)
{
	var_dump($k);
	var_dump($v);
}

echo "==SECOND==\n";

foreach(new RecursiveIteratorIterator($it, 1) as $k=>$v)
{
	var_dump($k);
	var_dump($v);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
==SECOND==
int(1)
array(3) {
  [0]=>
  int(21)
  [1]=>
  int(22)
  [2]=>
  array(1) {
    [0]=>
    int(231)
  }
}
int(2)
array(1) {
  [0]=>
  int(231)
}
===DONE===
