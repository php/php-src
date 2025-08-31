--TEST--
Weakrefs debug dump
--FILE--
<?php

$s = new stdClass;
$s->hello = 'world';

$weak = WeakReference::create($s);
var_dump($weak);
unset($s);
var_dump($weak);

?>
--EXPECT--
object(WeakReference)#2 (1) {
  ["object"]=>
  object(stdClass)#1 (1) {
    ["hello"]=>
    string(5) "world"
  }
}
object(WeakReference)#2 (1) {
  ["object"]=>
  NULL
}
