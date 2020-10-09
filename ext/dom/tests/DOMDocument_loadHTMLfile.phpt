--TEST--
Test DOMDocument::loadHTMLFile
--DESCRIPTION--
Verifies the basic behaviour of the method
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--INI--
assert.bail=true
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument();
$result = $doc->loadHTMLFile(__DIR__ . "/test.html");
assert($result === true);
?>
--EXPECT--
