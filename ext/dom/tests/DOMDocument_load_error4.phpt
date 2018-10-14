--TEST--
Test DOMDocument::load() detects not-well formed XML
--DESCRIPTION--
This test verifies the method detects an unsupported xml version
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
<?php if (LIBXML_VERSION < 20701) die("skip: libxml2 2.7.1+ required"); ?>
--ENV--
XML_FILE=/not_well_formed4.xml
LOAD_OPTIONS=0
EXPECTED_RESULT=0
--FILE_EXTERNAL--
domdocumentload_test_method.php
--EXPECTF--
Warning: DOMDocument::load%r(XML){0,1}%r(): Unsupported version '3.1' %s
