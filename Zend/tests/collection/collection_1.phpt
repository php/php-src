--TEST--
Collection: Syntax
--FILE--
<?php
collection(Dict) Articles(int => Article)
{
}

$c = new Articles;

var_dump($c);
?>
--EXPECTF--
object(Articles)#%d (%d) {
  ["value"]=>
  uninitialized(array)
}
