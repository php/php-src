--TEST--
Bug #81587: MultipleIterator Segmentation fault w/ SimpleXMLElement attached
--SKIPIF--
<?php if (!extension_loaded('simplexml')) die('skip requires simplexml'); ?>
--FILE--
<?php
$mi = new MultipleIterator();
$mi->attachIterator(new SimpleXMLElement('<x><y/></x>'));
foreach ($mi as $v) {
    var_dump($v);
}
?>
--EXPECT--
array(1) {
  [0]=>
  object(SimpleXMLElement)#4 (0) {
  }
}
