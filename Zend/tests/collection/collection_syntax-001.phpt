--TEST--
Collection: Syntax
--FILE--
<?php
collection(Dict) Articles<int => Article>
{
}

CoLleCtION(dICT) Books<string => Book>
{
}

$a = new Articles;
$b = new Books;

var_dump($a, $b);
?>
--EXPECTF--
object(Articles)#%d (%d) {
  ["value"]=>
  uninitialized(array)
}
object(Books)#%d (%d) {
  ["value"]=>
  uninitialized(array)
}
