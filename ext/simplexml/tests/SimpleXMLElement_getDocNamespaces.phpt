--TEST--
Testing getDocNamespaces() with invalid XML
--EXTENSIONS--
simplexml
--FILE--
<?php
$xml = @new SimpleXMLElement("X",1);
var_dump($xml->getDocNamespaces());
?>
--EXPECT--
bool(false)
