--TEST--
DOMNode::lookupPrefix()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString(<<<XML
<?xml version="1.0"?>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:x="test">
    <body>
        <svg:svg xmlns:svg="http://www.w3.org/2000/svg" height="1"/>
        <p xmlns:y="test">
            <x/>
        </p>
    </body>
</html>
XML);

$body = $dom->getElementsByTagName("body")[0];
$body->setAttribute("xmlns:a", "urn:a");

echo "--- NULL case because xmlns attribute not in xmlns namespace ---\n";

var_dump($body->lookupPrefix("urn:a"));

echo "--- svg case ---\n";

$svg = $dom->getElementsByTagNameNS("*", "svg")[0];

var_dump($svg->lookupPrefix(""));
var_dump($svg->lookupPrefix("http://www.w3.org/2000/svg"));
var_dump($svg->lookupPrefix("1"));

echo "--- search for \"test\" ---\n";

foreach (['x', 'p', 'html'] as $name) {
    $x = $dom->getElementsByTagNameNS("*", $name)[0];
    var_dump($x->lookupPrefix(""));
    var_dump($x->lookupPrefix("test"));
}

?>
--EXPECT--
--- NULL case because xmlns attribute not in xmlns namespace ---
NULL
--- svg case ---
NULL
string(3) "svg"
NULL
--- search for "test" ---
NULL
string(1) "y"
NULL
string(1) "y"
NULL
string(1) "x"
