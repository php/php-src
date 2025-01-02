--TEST--
GH-17145 (DOM memory leak)
--EXTENSIONS--
dom
--CREDITS--
YuanchengJiang
--SKIPIF--
<?php
if (LIBXML_VERSION < 21300) die("skip Upstream libxml bug causes incorrect output, fixed in GNOME/libxml2@b8597f4");
?>
--FILE--
<?php
$element = new DOMElement("N", "W", "y");
$attr = new DOMAttr("c" , "n");
$doc = new DOMDocument();
$doc->appendChild($element);
$element->setAttributeNodeNS($attr);
$attr->appendChild($doc->createEntityReference('amp'));
echo $attr->value;
?>
--EXPECT--
n&
