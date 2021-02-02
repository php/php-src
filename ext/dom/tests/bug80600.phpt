--TEST--
dom: DOMChildNode::remove does not work on character data
--FILE--
<?php

$doc = new \DOMDocument();
$doc->loadXML('<a><!-- foo --></a>');
$doc->documentElement->firstChild->remove();
echo $doc->saveXML($doc->documentElement);
--EXPECTF--
<a/>
