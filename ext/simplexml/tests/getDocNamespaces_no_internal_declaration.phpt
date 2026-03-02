--TEST--
getDocNamespaces() without internal namespace declaration in the document
--EXTENSIONS--
simplexml
dom
--FILE--
<?php

$xml = <<<XML
<root>
    <child xmlns="urn:a">
        <a/>
        <b xmlns=""/>
        <c xmlns:a="urn:a" xmlns="urn:c"/>
    </child>
    <child2 xmlns:d="urn:d"/>
</root>
XML;

$sxe = simplexml_load_string($xml);
var_dump($sxe->getDocNamespaces(true));

$dom = new DOMDocument;
$dom->loadXML($xml);
$sxe = simplexml_import_dom($dom);
var_dump($sxe->getDocNamespaces(true));

$dom = Dom\XMLDocument::createFromString($xml);
$sxe = simplexml_import_dom($dom);
var_dump($sxe->getDocNamespaces(true));

?>
--EXPECT--
array(3) {
  [""]=>
  string(5) "urn:a"
  ["a"]=>
  string(5) "urn:a"
  ["d"]=>
  string(5) "urn:d"
}
array(3) {
  [""]=>
  string(5) "urn:a"
  ["a"]=>
  string(5) "urn:a"
  ["d"]=>
  string(5) "urn:d"
}
array(3) {
  [""]=>
  string(5) "urn:a"
  ["a"]=>
  string(5) "urn:a"
  ["d"]=>
  string(5) "urn:d"
}
