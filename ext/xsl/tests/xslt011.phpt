--TEST--
Test 11: php:function Support
--EXTENSIONS--
xsl
--FILE--
<?php
print "Test 11: php:function Support\n";

$dom = new DOMDocument();
$dom->load(__DIR__."/xslt011.xsl");
$proc = new XSLTProcessor;
$xsl = $proc->importStylesheet($dom);

$xml = new DOMDocument();
$xml->load(__DIR__."/xslt011.xml");
$proc->registerPHPFunctions();
print $proc->transformToXml($xml);

function foobar($id, $secondArg = "" ) {
  if (is_array($id)) {
      return $id[0]->value . " - " . $secondArg;
  } else {
      return $id . " - " . $secondArg;
  }
}
function nodeSet($id = null) {
    if ($id and is_array($id)) {
        return $id[0];
    } else {
        $dom = new DOMDocument;
        $dom->loadXML("<root>this is from an external DomDocument</root>");
        return $dom->documentElement;
    }
}

class aClass {
  static function aStaticFunction($id) {
      return $id;
  }
}
?>
--EXPECTF--
Test 11: php:function Support
<?xml version="1.0"?>
foobar - secondArg
foobar - 
this is from an external DomDocument
from the Input Document
static
