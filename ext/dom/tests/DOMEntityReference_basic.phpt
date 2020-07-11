--TEST--
DOMEntityReference - read $nodeName property
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$der = new DOMEntityReference('nbsp');
var_dump($der->nodeName);
?>
--EXPECT--
string(4) "nbsp"
