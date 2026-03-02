--TEST--
xml_error_string() - Basic test on 5 error codes
--EXTENSIONS--
xml
--SKIPIF--
<?php
require __DIR__ . '/libxml_expat_skipif.inc';
skipif(want_expat: false);
?>
--FILE--
<?php
require __DIR__ . '/xml_error_string_basic.inc';
?>
--EXPECTF--
int(%r5|77%r)
string(%d) %r"Invalid document end"|"Tag not finished"%r
int(47)
string(35) "Processing Instruction not finished"
int(57)
string(28) "XML declaration not finished"
int(64)
string(17) "Reserved XML Name"
int(76)
string(14) "Mismatched tag"
