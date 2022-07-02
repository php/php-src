--TEST--
Test DOMDocument::load() with LIBXML_DTDVALID option
--DESCRIPTION--
This test verifies the right behaviour of the LIBXML_DTDVALID constant
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
XML_FILE=/wrong_book_with_dtd.xml
LOAD_OPTIONS=LIBXML_DTDVALID
EXPECTED_RESULT=1
--FILE_EXTERNAL--
domdocumentload_test_method.inc
--EXPECTF--
Warning: DOMDocument::load%r(XML){0,1}%r(): Element book content does not follow the DTD, expecting (title , author), got (title author author ) %s
