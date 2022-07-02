--TEST--
DOMEntityReference - read $nodeName property
--EXTENSIONS--
dom
--FILE--
<?php
$der = new DOMEntityReference('nbsp');
var_dump($der->nodeName);
?>
--EXPECT--
string(4) "nbsp"
