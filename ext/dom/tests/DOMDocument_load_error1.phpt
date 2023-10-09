--TEST--
Test DOMDocument::load() detects not-well formed XML
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
domdocumentload_test_method.inc
--EXPECTF--
Warning: DOMDocument::load%r(XML){0,1}%r(): Opening and ending tag mismatch: title line 5 and book %s

Warning: DOMDocument::load%r(XML){0,1}%r(): %rexpected '>'|Opening and ending tag mismatch: book line (4|5) and books%r %s

Warning: DOMDocument::load%r(XML){0,1}%r(): %rPremature end of data in tag books|EndTag: '<\/' not found%r %s
