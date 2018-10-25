--TEST--
Test DOMDocument::loadXML() detects not-well formed XML
--DESCRIPTION--
This test verifies the method detects a typo in tag names
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
XML_FILE=/not_well_formed3.xml
LOAD_OPTIONS=0
EXPECTED_RESULT=0
--FILE_EXTERNAL--
domdocumentloadxml_test_method.inc
--EXPECTF--
Warning: DOMDocument::load%r(XML){0,1}%r(): Opening and ending tag mismatch: boOk line 8 and book %s
