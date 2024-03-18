--TEST--
Element::removeAttributeNS()
--EXTENSIONS--
dom
--FILE--
<?php

function createFreshBody() {
    $dom = DOM\HTMLDocument::createFromString('<!DOCTYPE html><html><body align="foo" foo:bar="baz"></body></html>');
    $body = $dom->getElementsByTagName("body")[0];
    return $body;
}

echo "--- After parsing, i.e. without namespace ---\n";

$body = createFreshBody();
$body->removeAttributeNS(NULL, "align");
echo $body->ownerDocument->saveHTML($body), "\n";

$body = createFreshBody();
$body->removeAttributeNS("", "align");
echo $body->ownerDocument->saveHTML($body), "\n";

$body = createFreshBody();
$body->removeAttributeNS(NULL, "ALIGN");
echo $body->ownerDocument->saveHTML($body), "\n";

$body = createFreshBody();
$body->removeAttributeNS("", "ALIGN");
echo $body->ownerDocument->saveHTML($body), "\n";

$body = createFreshBody();
$body->removeAttributeNS(NULL, "foo:bar");
echo $body->ownerDocument->saveHTML($body), "\n";

$body = createFreshBody();
$body->removeAttributeNS("", "foo:bar");
echo $body->ownerDocument->saveHTML($body), "\n";

$body = createFreshBody();
$body->removeAttributeNS(NULL, "FOO:BAR");
echo $body->ownerDocument->saveHTML($body), "\n";

$body = createFreshBody();
$body->removeAttributeNS("", "FOO:BAR");
echo $body->ownerDocument->saveHTML($body), "\n";

echo "--- Special legacy case ---\n";

$body = createFreshBody();
$body->ownerDocument->documentElement->removeAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns");

echo "--- Remove after creating without namespace ---\n";

$body = createFreshBody();
$body->setAttributeNode($attr = $body->ownerDocument->createAttribute("prefix:local"));
$attr->value = "A";
echo $body->ownerDocument->saveHTML($body), "\n";
$body->removeAttributeNS(NULL, "prefix:LOCAL");
echo $body->ownerDocument->saveHTML($body), "\n";
$body->removeAttributeNS(NULL, "prefix:local");
echo $body->ownerDocument->saveHTML($body), "\n";

echo "--- Remove after creating with namespace ---\n";

$body = createFreshBody();
$body->setAttributeNode($attr = $body->ownerDocument->createAttributeNS("urn:a", "prefix:local2"));
$attr->value = "B";
echo $body->ownerDocument->saveHTML($body), "\n";
$body->removeAttributeNS("urn:a", "LOCAL2");
echo $body->ownerDocument->saveHTML($body), "\n";
$body->removeAttributeNS("urn:a", "local2");
echo $body->ownerDocument->saveHTML($body), "\n";

?>
--EXPECT--
--- After parsing, i.e. without namespace ---
<body foo:bar="baz"></body>
<body foo:bar="baz"></body>
<body align="foo" foo:bar="baz"></body>
<body align="foo" foo:bar="baz"></body>
<body align="foo"></body>
<body align="foo"></body>
<body align="foo" foo:bar="baz"></body>
<body align="foo" foo:bar="baz"></body>
--- Special legacy case ---
--- Remove after creating without namespace ---
<body align="foo" foo:bar="baz" prefix:local="A"></body>
<body align="foo" foo:bar="baz" prefix:local="A"></body>
<body align="foo" foo:bar="baz"></body>
--- Remove after creating with namespace ---
<body align="foo" foo:bar="baz" prefix:local2="B"></body>
<body align="foo" foo:bar="baz" prefix:local2="B"></body>
<body align="foo" foo:bar="baz"></body>
