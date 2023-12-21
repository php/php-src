--TEST--
DOMNamedNodeMap::getNamedItem()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<html>
<head>
    <title>Test</title>
</head>
<body align="foo" foo:bar="baz">
</body>
</html>
HTML);

echo "--- On HTML document ---\n";

$body = $dom->getElementsByTagName('body')->item(0);
$body->setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:foo", "http://example.com/foo");
$attributes = $body->attributes;

var_dump($attributes->getNamedItem("FOO:BAR")->value);
var_dump($attributes->getNamedItem("foo:BAR")->value);
var_dump($attributes->getNamedItem("foo:bar")->value);
var_dump($attributes->getNamedItem("XmLnS:foo")->value);
var_dump($attributes->getNamedItem("xmlns:foo")->value);

var_dump($attributes["FOO:BAR"]->value);
var_dump($attributes["foo:BAR"]->value);
var_dump($attributes["foo:bar"]->value);
var_dump($attributes["XmLnS:foo"]->value);
var_dump($attributes["xmlns:foo"]->value);

echo "--- On XML document ---\n";

$dom = DOM\XMLDocument::createEmpty();
$dom->appendChild($dom->importNode($body, true));

$body = $dom->getElementsByTagName('body')->item(0);
$attributes = $body->attributes;

var_dump($attributes->getNamedItem("FOO:BAR"));
var_dump($attributes->getNamedItem("foo:BAR"));
var_dump($attributes->getNamedItem("foo:bar")->value);
var_dump($attributes->getNamedItem("XmLnS:foo"));
var_dump($attributes->getNamedItem("xmlns:foo")->value);

var_dump($attributes["FOO:BAR"]);
var_dump($attributes["foo:BAR"]);
var_dump($attributes["foo:bar"]->value);
var_dump($attributes["XmLnS:foo"]);
var_dump($attributes["xmlns:foo"]->value);

?>
--EXPECT--
--- On HTML document ---
string(3) "baz"
string(3) "baz"
string(3) "baz"
string(22) "http://example.com/foo"
string(22) "http://example.com/foo"
string(3) "baz"
string(3) "baz"
string(3) "baz"
string(22) "http://example.com/foo"
string(22) "http://example.com/foo"
--- On XML document ---
NULL
NULL
string(3) "baz"
NULL
string(22) "http://example.com/foo"
NULL
NULL
string(3) "baz"
NULL
string(22) "http://example.com/foo"
