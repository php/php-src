--TEST--
Interoperability with SimpleXML
--EXTENSIONS--
dom
simplexml
--FILE--
<?php

$sxe = simplexml_load_string('<container xmlns="urn:a">foo</container>');

$element = Dom\import_simplexml($sxe);
var_dump($element->attributes);
echo $element->ownerDocument->saveXml($element), "\n";

$element->appendChild($element->ownerDocument->createElementNS('urn:a', 'child'));
echo $element->ownerDocument->saveXml($element), "\n";

$sxe->addChild('name', 'value');
echo $element->ownerDocument->saveXml($element), "\n";

// This should fail because it has been imported already above in modern DOM
try {
    dom_import_simplexml($sxe);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

// Import again and compare
var_dump(Dom\import_simplexml($sxe) === $element);

?>
--EXPECT--
object(Dom\NamedNodeMap)#3 (1) {
  ["length"]=>
  int(1)
}
<container xmlns="urn:a">foo</container>
<container xmlns="urn:a">foo<child/></container>
<container xmlns="urn:a">foo<child/><name>value</name></container>
dom_import_simplexml(): Argument #1 ($node) must not be already imported as a Dom\Node
bool(true)
