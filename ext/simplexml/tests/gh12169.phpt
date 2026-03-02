--TEST--
GH-12169 (Unable to get comment contents in SimpleXML)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = <<<XML
<?xml version="1.0"?>
<container>
    <!-- comment contents -->
</container>
XML;

$sxe = simplexml_load_string($xml);

var_dump($sxe->xpath("//comment()")[0]->getName());
var_dump((string) $sxe->xpath("//comment()")[0]);

?>
--EXPECT--
string(7) "comment"
string(18) " comment contents "
