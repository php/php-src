--TEST--
Test DOMDocument::loadXML() detects not-well formed XML
--SKIPIF--
<?php
if (LIBXML_VERSION >= 21100) die('skip libxml2 test variant for version < 2.11');
?>
--DESCRIPTION--
This test verifies the method detects attributes values not closed between " or '
Environment variables used in the test:
- XML_FILE: the xml file to load
- LOAD_OPTIONS: the second parameter to pass to the method
- EXPECTED_RESULT: the expected result
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--INI--
assert.bail=true
--EXTENSIONS--
dom
--ENV--
XML_FILE=/not_well_formed2.xml
LOAD_OPTIONS=0
EXPECTED_RESULT=0
--FILE_EXTERNAL--
domdocumentloadxml_test_method.inc
--EXPECTF--
Warning: DOMDocument::load%r(XML){0,1}%r(): AttValue: " or ' expected %s

Warning: DOMDocument::load%r(XML){0,1}%r(): attributes construct error %s

Warning: DOMDocument::load%r(XML){0,1}%r(): Couldn't find end of Start Tag book %s

Warning: DOMDocument::load%r(XML){0,1}%r(): Opening and ending tag mismatch: books %s

Warning: DOMDocument::load%r(XML){0,1}%r(): Extra content at the end of the document %s
