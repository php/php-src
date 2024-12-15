--TEST--
GH-17153 (SimpleXML crash when using autovivification on document)
--EXTENSIONS--
dom
simplexml
xsl
--FILE--
<?php
$sxe = simplexml_load_file(__DIR__ . '/../../xsl/tests/53965/collection.xml', SimpleXMLElement::class);
$processor = new XSLTProcessor;
$dom = new DOMDocument;
$dom->load(__DIR__ . '/../../xsl/tests/53965/collection.xsl');
$processor->importStylesheet($dom);
$result = $processor->transformToDoc($sxe, SimpleXMLElement::class);
$result->h = "x";
var_dump($result);
?>
--EXPECT--
object(SimpleXMLElement)#4 (4) {
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
    object(SimpleXMLElement)#5 (0) {
    }
    [1]=>
    object(SimpleXMLElement)#6 (0) {
    }
  }
  ["h"]=>
  string(1) "x"
}
