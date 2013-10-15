--TEST--
Test DOMDocument::loadXML() basic behavior
--DESCRIPTION--
This test verifies the basic behaviour of the method
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
XML_FILE=/book.xml
LOAD_OPTIONS=0
EXPECTED_RESULT=1
--FILE_EXTERNAL--
domdocumentloadxml_test_method.php
--EXPECT--
