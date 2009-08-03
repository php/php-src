--TEST--
Bug #43221 (SimpleXML adding default namespace in addAttribute)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml = simplexml_load_string('<?xml version="1.0" encoding="utf-8"?><root />');
$n = $xml->addChild("node", "value");
$n->addAttribute("a", "b");
$n->addAttribute("c", "d", "http://bar.com");
$n->addAttribute("foo:e", "f", "http://bar.com");
print_r($xml->asXml());
?>
===DONE===
--EXPECTF--
Warning: SimpleXMLElement::addAttribute(): Attribute requires prefix for namespace in %sbug43221.php on line %d
<?xml version="1.0" encoding="utf-8"?>
<root><node xmlns:foo="http://bar.com" a="b" foo:e="f">value</node></root>
===DONE===
	