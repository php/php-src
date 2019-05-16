--TEST--
Bug #73686 (Adding settype()ed values to ArrayObject results in references)
--FILE--
<?php

$ao = new ArrayObject;

foreach ([1, 2, 3] as $i => $var)
{
	settype($var, 'string');
	$ao[$i] = $var;
}
var_dump($ao);

$ao = new ArrayObject;

foreach ([1, 2, 3] as $i => $var)
{
	$ao[$i] = &$var;
}
var_dump($ao);
?>
--EXPECTF--
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
  }
}
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    &int(3)
    [1]=>
    &int(3)
    [2]=>
    &int(3)
  }
}
