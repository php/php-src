--TEST--
Bug #39662 (Segfault when calling asXML() of a cloned SimpleXMLElement)
--EXTENSIONS--
simplexml
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
object(SimpleXMLElement)#%d (0) {
}
object(SimpleXMLElement)#%d (0) {
}
string(%d) "<?xml version="1.0" encoding="utf-8"?>
<test>

</test>
"
Done
