--TEST--
Bug #39662 (Segfault when calling asXML() of a cloned SimpleXMLElement)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip simplexml extension is not loaded"; ?>
--FILE--
<?php

$xml = '<?xml version="1.0" encoding="utf-8" ?>
<test>

</test>';

$root = simplexml_load_string($xml);
$clone = clone $root;
var_dump($root);
var_dump($clone);
var_dump($clone->asXML());

echo "Done\n";
?>
--EXPECTF--
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(2) "

"
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(2) "

"
}
string(15) "<test>

</test>"
Done
