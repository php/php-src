--TEST--
DOMNode::isEqualNode edge cases 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$dom->append($dom->createElement("container"));
$e1 = $dom->documentElement->appendChild($dom->createElement("foo1"));
$e1->after("\n");
$e2 = $dom->documentElement->appendChild($dom->createElement("foo2"));

$e2->setAttribute("bar1", "baz");
$e2->setAttribute("bar2", "baz");
// Internal namespace node creation
$e2->setAttributeNodeNS($child = $dom->createAttributeNS("http://example.com", "foo:bar"));
$e2->removeAttributeNode($child);

$dom2 = DOM\HTMLDocument::createFromString("<!DOCTYPE HTML>" . $dom->saveHTML());

var_dump($dom->saveHTML($dom->getElementsByTagName("container")[0]));
var_dump($dom2->saveHTML($dom2->getElementsByTagName("container")[0]));
var_dump($dom->getElementsByTagName("container")[0]->isEqualNode($dom2->getElementsByTagName("container")[0]));

?>
--EXPECT--
string(72) "<container><foo1></foo1>
<foo2 bar1="baz" bar2="baz"></foo2></container>"
string(72) "<container><foo1></foo1>
<foo2 bar1="baz" bar2="baz"></foo2></container>"
bool(true)
