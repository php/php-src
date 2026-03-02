--TEST--
GH-15551 (Segmentation fault (access null pointer) in ext/dom/xml_common.h)
--EXTENSIONS--
dom
--FILE--
<?php
$fragment = new DOMDocumentFragment();
$nodes = $fragment->childNodes;
$iter = $nodes->getIterator();
$iter->next();
var_dump($iter->valid());
?>
--EXPECT--
bool(false)
