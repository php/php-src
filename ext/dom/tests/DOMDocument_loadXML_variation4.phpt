--TEST--
Test DOMDocument::loadXML() with LIBXML_DTDATTR, LIBXML_NOCDATA, LIBXML_NOENT, LIBXML_NOBLANKS
--DESCRIPTION--
This test verifies the right behaviour of the following constants:
LIBXML_DTDATTR, LIBXML_NOCDATA, LIBXML_NOENT and LIBXML_NOBLANKS
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
XML_FILE=/book_with_dtd2.xml
LOAD_OPTIONS=LIBXML_DTDATTR|LIBXML_NOCDATA|LIBXML_NOENT|LIBXML_NOBLANKS
EXPECTED_RESULT=1
--FILE_EXTERNAL--
domdocumentloadxml_test_method_savexml.php
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE books [
<!ENTITY entitest "entity is only for test purposes">
<!ATTLIST title default CDATA "default title">
<!ELEMENT books (book)*>
<!ELEMENT book (title , author)>
<!ELEMENT title (#PCDATA)>
<!ELEMENT author (#PCDATA)>
]>
<books><book><title default="default title">The Grapes of Wrath</title><author>John Steinbeck</author></book><book><title default="default title">The Pearl</title><author>John Steinbeck</author></book><book><title default="default title">entity is only for test purposes</title><author>data for test</author></book></books>
