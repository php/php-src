--TEST--
Test DOMDocument::loadXML() detects not-well formed XML
--SKIPIF--
<?php
if (LIBXML_VERSION < 21100) die('skip libxml2 test variant for version >= 2.11');
?>
--DESCRIPTION--
This test verifies the method detects attributes values not closed between " or '
Environment variables used in the test:
- XML_FILE: the xml file to load
- LOAD_OPTIONS: the second parameter to pass to the method
- EXPECTED_RESULT: the expected result
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--EXTENSIONS--
dom
--ENV--
XML_FILE=/not_well_formed2.xml
LOAD_OPTIONS=0
EXPECTED_RESULT=0
--FILE_EXTERNAL--
domdocumentloadxml_test_method.inc
--EXPECTF--
Warning: DOMDocument::loadXML(): AttValue: " or ' expected in Entity, line: 4 in %s on line %d

Warning: DOMDocument::loadXML(): internal error: xmlParseStartTag: problem parsing attributes in Entity, line: 4 in %s on line %d

Warning: DOMDocument::loadXML(): Couldn't find end of Start Tag book line 4 in Entity, line: 4 in %s on line %d

Warning: DOMDocument::loadXML(): Opening and ending tag mismatch: books line 3 and book in Entity, line: 7 in %s on line %d

Warning: DOMDocument::loadXML(): Extra content at the end of the document in Entity, line: 8 in %s on line %d
