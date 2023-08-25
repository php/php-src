--TEST--
DOM\HTML5Document::registerNodeClass 03
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->registerNodeClass("DOMDocument", "DOM\\HTML5Document");

$element = $dom->appendChild($dom->createElement("foo"));
unset($dom);

var_dump(get_class($element->ownerDocument));

$dom = $element->ownerDocument;
unset($element);
$element = $dom->documentElement;
unset($dom);

var_dump(get_class($element->ownerDocument));

?>
--EXPECT--
string(17) "DOM\HTML5Document"
string(17) "DOM\HTML5Document"
