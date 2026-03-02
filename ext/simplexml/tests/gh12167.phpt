--TEST--
GH-12167 (Unable to get processing instruction contents in SimpleXML)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = <<<XML
<?xml version="1.0"?>
<container>
  <?foo pi contents ?>
</container>
XML;

$sxe = simplexml_load_string($xml);

var_dump($sxe->xpath("//processing-instruction()")[0]->getName());
var_dump((string) $sxe->xpath("//processing-instruction()")[0]);

?>
--EXPECT--
string(3) "foo"
string(12) "pi contents "
