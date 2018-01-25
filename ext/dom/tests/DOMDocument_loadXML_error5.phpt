--TEST--
Test DOMDocument::loadXML() detects not-well formed XML
--DESCRIPTION--
This test verifies the method detects extra content at the end of the document
Environment variables used in the test:
- XML_FILE: the xml file to load
- LOAD_OPTIONS: the second parameter to pass to the method
- EXPECTED_RESULT: the expected result
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--INI--
assert.bail=true
--SKIPIF--
<?php include('skipif.inc'); ?>
--ENV--
XML_FILE=/not_well_formed5.xml
LOAD_OPTIONS=0
EXPECTED_RESULT=0
--FILE_EXTERNAL--
domdocumentloadxml_test_method.php
--EXPECTF--
Warning: DOMDocument::load%r(XML){0,1}%r(): Extra content at the end of the document %s
