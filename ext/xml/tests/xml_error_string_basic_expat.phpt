--TEST--
xml_error_string() - Basic test on 5 error codes
--EXTENSIONS--
xml
--SKIPIF--
<?php
require __DIR__ . '/libxml_expat_skipif.inc';
skipif(want_expat: true);
?>
--FILE--
<?php
require __DIR__ . '/xml_error_string_basic.inc';
?>
--EXPECT--
int(3)
string(16) "no element found"
int(4)
string(31) "not well-formed (invalid token)"
int(5)
string(14) "unclosed token"
int(30)
string(31) "XML declaration not well-formed"
int(7)
string(14) "mismatched tag"
