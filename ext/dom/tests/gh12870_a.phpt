--TEST--
GH-12870 (Creating an xmlns attribute results in a DOMException) - xmlns variations
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/gh12870.inc';

echo "=== NORMAL CASES ===\n";

test('http://www.w3.org/2000/xmlns/qx', 'foo:xmlns');
test('http://www.w3.org/2000/xmlns/', 'xmlns');
test('http://www.w3.org/2000/xmlns/', 'xmlns:xmlns');

echo "=== ERROR CASES ===\n";

test('http://www.w3.org/2000/xmlns/', 'bar:xmlns');
test('http://www.w3.org/2000/xmlns/a', 'xmlns');
test('http://www.w3.org/2000/xmlns/a', 'xmlns:bar');
test(null, 'xmlns:bar');
test('', 'xmlns');
test('http://www.w3.org/2000/xmlns/', '');

?>
--EXPECT--
=== NORMAL CASES ===
--- Testing "http://www.w3.org/2000/xmlns/qx", "foo:xmlns" ---
Attr prefix: string(3) "foo"
Attr namespaceURI: string(31) "http://www.w3.org/2000/xmlns/qx"
Attr value: string(0) ""
root namespaceURI: NULL
Equality check: bool(true)
<?xml version="1.0"?>
<root xmlns:foo="http://www.w3.org/2000/xmlns/qx" foo:xmlns=""/>

--- Testing "http://www.w3.org/2000/xmlns/", "xmlns" ---
Attr prefix: string(0) ""
Attr namespaceURI: string(29) "http://www.w3.org/2000/xmlns/"
Attr value: string(0) ""
root namespaceURI: NULL
Equality check: bool(true)
<?xml version="1.0"?>
<root xmlns=""/>

--- Testing "http://www.w3.org/2000/xmlns/", "xmlns:xmlns" ---
Attr prefix: string(5) "xmlns"
Attr namespaceURI: string(29) "http://www.w3.org/2000/xmlns/"
Attr value: string(0) ""
root namespaceURI: NULL
Equality check: bool(true)
<?xml version="1.0"?>
<root xmlns:xmlns="http://www.w3.org/2000/xmlns/" xmlns:xmlns=""/>

=== ERROR CASES ===
--- Testing "http://www.w3.org/2000/xmlns/", "bar:xmlns" ---
Exception: Namespace Error

--- Testing "http://www.w3.org/2000/xmlns/a", "xmlns" ---
Exception: Namespace Error

--- Testing "http://www.w3.org/2000/xmlns/a", "xmlns:bar" ---
Exception: Namespace Error

--- Testing NULL, "xmlns:bar" ---
Exception: Namespace Error

--- Testing "", "xmlns" ---
Exception: Namespace Error

--- Testing "http://www.w3.org/2000/xmlns/", "" ---
Exception: Namespace Error
