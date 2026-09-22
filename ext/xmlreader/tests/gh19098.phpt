--TEST--
GH-19098 (libxml<2.13 segmentation fault caused by php_libxml_node_free)
--EXTENSIONS--
xmlreader
dom
--FILE--
<?php

$xml_reader = \XMLReader::XML('
<sparql xmlns="http://www.w3.org/2005/sparql-results#">
 <results>
  <result><binding xml:id="foo" xmlns:custom="urn:custom" custom:foo="bar" name="s"><uri/></binding></result>
 </results>
</sparql>');

$success = $xml_reader->next("sparql");

$success = $xml_reader->read();
$success = $xml_reader->next("results");

while ($xml_reader->read()) {
  if ($xml_reader->next("result")) {
    $result_as_dom_node = $xml_reader->expand();
    $child = $result_as_dom_node->firstChild;
    unset($result_as_dom_node);
    var_dump($child->namespaceURI);
    foreach ($child->attributes as $attr) {
      var_dump($attr->namespaceURI);
    }
    $doc = new DOMDocument;
    $doc->adoptNode($child);
    echo $doc->saveXML($child), "\n";
    unset($child);
    break;
  }
}

?>
--EXPECT--
string(38) "http://www.w3.org/2005/sparql-results#"
string(36) "http://www.w3.org/XML/1998/namespace"
string(10) "urn:custom"
NULL
<default:binding xmlns:custom="urn:custom" xmlns:default="http://www.w3.org/2005/sparql-results#" xml:id="foo" custom:foo="bar" name="s"><default:uri/></default:binding>
