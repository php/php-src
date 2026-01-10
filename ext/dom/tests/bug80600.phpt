--TEST--
dom: DOMChildNode::remove does not work on character data
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new \DOMDocument();
$doc->loadXML('<a><!-- foo --></a>');
$doc->documentElement->firstChild->remove();
echo $doc->saveXML($doc->documentElement);
?>
--EXPECT--
<a/>
