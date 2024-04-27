--TEST--
DOM-Parsing GH-45 (It's possible that 'retrieve a preferred prefix string' returns a wrong prefix for the specified namespace)
--EXTENSIONS--
dom
--FILE--
<?php

$root = DOM\XMLDocument::createFromString('<el1 xmlns:p="u1" xmlns:q="u1"><el2 xmlns:q="u2"/></el1>')->documentElement;
$root->firstChild->setAttributeNS('u1', 'name', 'v');
$attr = $root->firstChild->getAttributeNodeNS("u1", "name");
var_dump($attr->prefix, $attr->namespaceURI);
echo $root->ownerDocument->saveXML($root), "\n";

?>
--EXPECT--
NULL
string(2) "u1"
<el1 xmlns:p="u1" xmlns:q="u1"><el2 xmlns:q="u2" p:name="v"/></el1>
