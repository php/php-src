--TEST--
Test DOMDocument::load() with LIBXML_DTDLOAD option
--DESCRIPTION--
This test verifies the right behaviour of the LIBXML_DTDLOAD constant
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
XML_FILE=/book_with_dtd.xml
LOAD_OPTIONS=LIBXML_DTDLOAD
EXPECTED_RESULT=1
--FILE_EXTERNAL--
domdocumentload_test_method.inc
--EXPECT--
