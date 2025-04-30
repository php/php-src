--TEST--
Node renaming
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString(<<<XML
<root xmlns:a="urn:a">
    <a:child attrib="value"/>
</root>
XML);

function test($target, $namespaceURI, $qualifiedName) {
    $namespaceURIPretty = json_encode($namespaceURI);
    $qualifiedNamePretty = json_encode($qualifiedName);
    echo "--- rename to $namespaceURIPretty $qualifiedNamePretty ---\n";
    $target->rename($namespaceURI, $qualifiedName);
    echo $target->ownerDocument->saveXML(), "\n";
    var_dump($target->namespaceURI, $target->prefix);
}

echo "=== Element test ===\n";

test($dom->documentElement, "urn:x", "x:foo");
test($dom->documentElement, "urn:x", "a:foo");
test($dom->documentElement, "", "foo");
test($dom->documentElement, null, "bar");

echo "=== Attribute test ===\n";

$attribute = $dom->documentElement->firstElementChild->attributes[0];

test($attribute, "urn:x", "x:foo");
test($attribute, "urn:x", "a:foo");
test($attribute, "", "foo");
test($attribute, null, "bar");

?>
--EXPECT--
=== Element test ===
--- rename to "urn:x" "x:foo" ---
<?xml version="1.0" encoding="UTF-8"?>
<x:foo xmlns:x="urn:x" xmlns:a="urn:a">
    <a:child attrib="value"/>
</x:foo>
string(5) "urn:x"
string(1) "x"
--- rename to "urn:x" "a:foo" ---
<?xml version="1.0" encoding="UTF-8"?>
<ns1:foo xmlns:ns1="urn:x" xmlns:a="urn:a">
    <a:child attrib="value"/>
</ns1:foo>
string(5) "urn:x"
string(1) "a"
--- rename to "" "foo" ---
<?xml version="1.0" encoding="UTF-8"?>
<foo xmlns:a="urn:a">
    <a:child attrib="value"/>
</foo>
NULL
NULL
--- rename to null "bar" ---
<?xml version="1.0" encoding="UTF-8"?>
<bar xmlns:a="urn:a">
    <a:child attrib="value"/>
</bar>
NULL
NULL
=== Attribute test ===
--- rename to "urn:x" "x:foo" ---
<?xml version="1.0" encoding="UTF-8"?>
<bar xmlns:a="urn:a">
    <a:child xmlns:x="urn:x" x:foo="value"/>
</bar>
string(5) "urn:x"
string(1) "x"
--- rename to "urn:x" "a:foo" ---
<?xml version="1.0" encoding="UTF-8"?>
<bar xmlns:a="urn:a">
    <a:child xmlns:a="urn:x" a:foo="value"/>
</bar>
string(5) "urn:x"
string(1) "a"
--- rename to "" "foo" ---
<?xml version="1.0" encoding="UTF-8"?>
<bar xmlns:a="urn:a">
    <a:child foo="value"/>
</bar>
NULL
NULL
--- rename to null "bar" ---
<?xml version="1.0" encoding="UTF-8"?>
<bar xmlns:a="urn:a">
    <a:child bar="value"/>
</bar>
NULL
NULL
