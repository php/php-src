--TEST--
Test DOMDocument::loadHTMLFile when a not-well formed document is loaded
--DESCRIPTION--
Verifies the behavior if a not-well formed document is loaded
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--INI--
assert.bail=true
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument();
$result = $doc->loadHTMLFile(dirname(__FILE__) . "/not_well.html");
assert($result === true);
?>
--EXPECT--
