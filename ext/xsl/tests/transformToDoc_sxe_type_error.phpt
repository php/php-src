--TEST--
XSLTProcessor::transformToDoc return value type error with SimpleXML
--EXTENSIONS--
xsl
--FILE--
<?php

class AdvancedXMLElement extends SimpleXMLElement {
    public function foo() {
        return "foo: " . (string) $this;
    }
}

$sxe = simplexml_load_file(__DIR__ . '/53965/collection.xml', AdvancedXMLElement::class);

$processor = new XSLTProcessor;
$dom = new DOMDocument;
$dom->load(__DIR__ . '/53965/collection.xsl');
$processor->importStylesheet($dom);
$result = $processor->transformToDoc($sxe, AdvancedXMLElement::class);

var_dump($result);
var_dump($result->h1->foo());

?>
--EXPECT--
object(AdvancedXMLElement)#4 (3) {
  ["h1"]=>
  array(2) {
    [0]=>
    string(19) "Fight for your mind"
    [1]=>
    string(17) "Electric Ladyland"
  }
  ["h2"]=>
  array(2) {
    [0]=>
    string(20) "by Ben Harper - 1995"
    [1]=>
    string(22) "by Jimi Hendrix - 1997"
  }
  ["hr"]=>
  array(2) {
    [0]=>
    object(AdvancedXMLElement)#5 (0) {
    }
    [1]=>
    object(AdvancedXMLElement)#6 (0) {
    }
  }
}
string(24) "foo: Fight for your mind"
