--TEST--
HTMLDocument::getElementsByTagName
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . "/node_list_dump.inc";
require __DIR__ . "/create_element_util.inc";

$dom = DOM\HTMLDocument::createEmpty();
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
// Matches 6 instead of 7 because the input to this function is lowercased
// _and_ createElement also lowercases.
// This is in contrast to createElementNS that won't lowercase.
dumpNodeList($dom->getElementsByTagName("foo:html"));
// This will match both 6 and 7. 7 is now matched because the casing is right.
dumpNodeList($dom->getElementsByTagName("foo:HTML"));
// Both of the following calls will match 9 because 8 isn't lowercased due to the use of createElementNS.
dumpNodeList($dom->getElementsByTagName("bar:HTML"));
dumpNodeList($dom->getElementsByTagName("bar:html"));

?>
--EXPECT--
Node list length: int(2)
	HTML 1
	HTML 2
---
Node list length: int(3)
	HTML 1
	HTML 2
	htML 5
---
Node list length: int(3)
	HTML 1
	HTML 2
	html 3
---
Node list length: int(3)
	HTML 1
	HTML 2
	HTML 4
---
Node list length: int(1)
	FOO:HTML 6
---
Node list length: int(2)
	FOO:HTML 6
	foo:HTML 7
---
Node list length: int(1)
	BAR:HTML 9
---
Node list length: int(1)
	BAR:HTML 9
---
