--TEST--
Testing getDocNamespaces() with invalid XML
--FILE--
<?php
$xml = @new SimpleXMLElement("X",1);
var_dump($xml->getDocNamespaces());
?>
--EXPECTF--
bool(false)
