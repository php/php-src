--TEST--
GH-12870 (Creating an xmlns attribute results in a DOMException) - xml variations
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/gh12870.inc';

echo "=== NORMAL CASES ===\n";

test('http://www.w3.org/XML/1998/namespaceqx', 'foo:xml');
test('http://www.w3.org/XML/1998/namespace', 'xml');
test('http://www.w3.org/XML/1998/namespace', 'bar:xml');
test('', 'xml');
test('http://www.w3.org/XML/1998/namespacea', 'xml');

echo "=== ERROR CASES ===\n";

test('http://www.w3.org/XML/1998/namespace', 'xmlns:xml');
test('http://www.w3.org/XML/1998/namespace', '');
test('http://www.w3.org/XML/1998/namespacea', 'xml:foo');
test(NULL, 'xml:foo');

?>
--EXPECT--
=== NORMAL CASES ===
--- Testing "http://www.w3.org/XML/1998/namespaceqx", "foo:xml" ---
Attr prefix: string(3) "foo"
Attr namespaceURI: string(38) "http://www.w3.org/XML/1998/namespaceqx"
Attr value: string(0) ""
root namespaceURI: NULL
Equality check: bool(true)
<?xml version="1.0"?>
<root xmlns:foo="http://www.w3.org/XML/1998/namespaceqx" foo:xml=""/>

--- Testing "http://www.w3.org/XML/1998/namespace", "xml" ---
Attr prefix: string(3) "xml"
Attr namespaceURI: string(36) "http://www.w3.org/XML/1998/namespace"
Attr value: string(0) ""
root namespaceURI: NULL
Equality check: bool(true)
<?xml version="1.0"?>
<root xml:xml=""/>

--- Testing "http://www.w3.org/XML/1998/namespace", "bar:xml" ---
Attr prefix: string(3) "xml"
Attr namespaceURI: string(36) "http://www.w3.org/XML/1998/namespace"
Attr value: string(0) ""
root namespaceURI: NULL
Equality check: bool(true)
<?xml version="1.0"?>
<root xml:xml=""/>

--- Testing "", "xml" ---
Attr prefix: string(0) ""
Attr namespaceURI: NULL
Attr value: string(0) ""
root namespaceURI: NULL
Equality check: bool(false)
<?xml version="1.0"?>
<root xml=""/>

--- Testing "http://www.w3.org/XML/1998/namespacea", "xml" ---
Attr prefix: string(7) "default"
Attr namespaceURI: string(37) "http://www.w3.org/XML/1998/namespacea"
Attr value: string(0) ""
root namespaceURI: NULL
Equality check: bool(true)
<?xml version="1.0"?>
<root xmlns:default="http://www.w3.org/XML/1998/namespacea" default:xml=""/>

=== ERROR CASES ===
--- Testing "http://www.w3.org/XML/1998/namespace", "xmlns:xml" ---
Exception: Namespace Error

--- Testing "http://www.w3.org/XML/1998/namespace", "" ---
Exception: Namespace Error

--- Testing "http://www.w3.org/XML/1998/namespacea", "xml:foo" ---
Exception: Namespace Error

--- Testing NULL, "xml:foo" ---
Exception: Namespace Error
