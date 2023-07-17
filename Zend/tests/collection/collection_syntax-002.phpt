--TEST--
Collection: Sequence syntax with scalars
--FILE--
<?php
collection(Seq) Titles<string>
{
}

$c = new Titles;

var_dump($c);
?>
--EXPECTF--
object(Titles)#%d (%d) {
  ["value"]=>
  uninitialized(array)
}
