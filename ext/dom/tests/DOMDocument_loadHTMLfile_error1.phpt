--TEST--
Test DOMDocument::loadHTMLFile when the file doesn't exist
--DESCRIPTION--
Verifies that an error message is showed if the file doesn't exist
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--INI--
assert.bail=true
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument();
$result = $doc->loadHTMLFile(dirname(__FILE__) . "/ffff/test.html");
assert($result === false);
?>
--EXPECTF--
%r(PHP ){0,1}%rWarning: DOMDocument::loadHTMLFile(): I/O warning : failed to load external entity %s
