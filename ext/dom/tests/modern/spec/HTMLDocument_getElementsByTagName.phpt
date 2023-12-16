--TEST--
HTMLDocument::getElementsByTagName
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . "/node_list_dump.inc";

$dom = DOM\HTMLDocument::createEmpty();
$dom->appendChild($dom->createElement("HTML", "1"));
$dom->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "html", "2"));
$dom->appendChild($dom->createElementNS(NULL, "html", "3"));
$dom->appendChild($dom->createElementNS(NULL, "HTML", "4"));
$dom->appendChild($dom->createElementNS("urn:foo", "htML", "5"));
$dom->appendChild($dom->createElement("foo:HTML", "6"));
$dom->appendChild($dom->createElementNS("urn:a", "foo:HTML", "7")); // Should never match in this test file
$dom->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "bar:HTML", "8"));
$dom->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "bar:html", "9"));

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
