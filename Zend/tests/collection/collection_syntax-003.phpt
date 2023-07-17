--TEST--
Collection: Syntax with 'Collection' class
--FILE--
<?php
class Collection {}
class Article extends Collection {}

collection(SEQ) Articles<Article>
{
}

$a = new Articles;

var_dump($a);
?>
--EXPECTF--
object(Articles)#%d (%d) {
  ["value"]=>
  uninitialized(array)
}
