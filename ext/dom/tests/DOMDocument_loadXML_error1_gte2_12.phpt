--TEST--
Test DOMDocument::loadXML() detects not-well formed XML
--SKIPIF--
<?php
if (LIBXML_VERSION < 21200) die('skip libxml2 test variant for version >= 2.12');
?>
--DESCRIPTION--
This test verifies the method detects an opening and ending tag mismatch
Environment variables used in the test:
- XML_FILE: the xml file to load
- LOAD_OPTIONS: the second parameter to pass to the method
- EXPECTED_RESULT: the expected result
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--EXTENSIONS--
dom
--ENV--
XML_FILE=/not_well_formed.xml
LOAD_OPTIONS=0
EXPECTED_RESULT=0
--FILE_EXTERNAL--
domdocumentloadxml_test_method.inc
--EXPECTF--
Warning: DOMDocument::load%r(XML){0,1}%r(): Opening and ending tag mismatch: title line 5 and book %s

Warning: DOMDocument::load%r(XML){0,1}%r(): %rexpected '>'|Opening and ending tag mismatch: book line (4|5) and books%r %s
