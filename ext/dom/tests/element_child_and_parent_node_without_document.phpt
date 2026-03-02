--TEST--
DOMElement: DOMChildNode, DOMParentNode modifications without a document
--EXTENSIONS--
dom
--FILE--
<?php

$element = new DOMElement("p", " Hello World! ");
$element->append("APPENDED");
$element->prepend("PREPENDED");
$element->after("AFTER");
$element->before("BEFORE");
$element->replaceWith("REPLACE");

$doc = new DOMDocument();
$doc->adoptNode($element);
echo $doc->saveXML($element), "\n";

?>
--EXPECT--
<p>PREPENDED Hello World! APPENDED</p>
