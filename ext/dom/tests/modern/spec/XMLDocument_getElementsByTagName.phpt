--TEST--
XMLDocument::getElementsByTagName
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . "/node_list_dump.inc";
require __DIR__ . "/create_element_util.inc";

$dom = DOM\XMLDocument::createEmpty();
$container = $dom->appendChild(createElement($dom, "container"));
$container->appendChild(createElement($dom, "HTML", "1"));
$container->appendChild(createElementNS($dom, "http://www.w3.org/1999/xhtml", "html", "2"));
$container->appendChild(createElementNS($dom, NULL, "html", "3"));
$container->appendChild(createElementNS($dom, NULL, "HTML", "4"));
$container->appendChild(createElementNS($dom, "urn:foo", "htML", "5"));
$container->appendChild(createElement($dom, "foo:HTML", "6"));
$container->appendChild(createElementNS($dom, "urn:a", "foo:HTML", "7")); // Should never match in this test file
$container->appendChild(createElementNS($dom, "http://www.w3.org/1999/xhtml", "bar:HTML", "8"));
$container->appendChild(createElementNS($dom, "http://www.w3.org/1999/xhtml", "bar:html", "9"));

dumpNodeList($dom->getElementsByTagName("HTml"));
dumpNodeList($dom->getElementsByTagName("htML"));
dumpNodeList($dom->getElementsByTagName("html"));
dumpNodeList($dom->getElementsByTagName("HTML"));
dumpNodeList($dom->getElementsByTagName("foo:html"));
dumpNodeList($dom->getElementsByTagName("foo:HTML"));
dumpNodeList($dom->getElementsByTagName("bar:HTML"));
dumpNodeList($dom->getElementsByTagName("bar:html"));

?>
--EXPECT--
Node list length: int(0)
---
Node list length: int(1)
	htML 5
---
Node list length: int(2)
	html 2
	html 3
---
Node list length: int(2)
	HTML 1
	HTML 4
---
Node list length: int(0)
---
Node list length: int(2)
	foo:HTML 6
	foo:HTML 7
---
Node list length: int(1)
	bar:HTML 8
---
Node list length: int(1)
	bar:html 9
---
