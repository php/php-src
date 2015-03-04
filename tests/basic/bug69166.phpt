--TEST--
Bug #69166 Assigning array_values() to array does not reset key counter
--FILE--
<?php 
$ar = array('foo');
unset($ar[0]);
$ar = array_values($ar);
$ar[] = 'bar';
var_dump($ar);

?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "bar"
}
