--TEST--
Test DOMDocument::loadHTMLFile when an empty document is loaded
--DESCRIPTION--
Verifies that an warning message is showed if an empty document is loaded
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--INI--
assert.bail=true
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument();
$result = $doc->loadHTMLFile(dirname(__FILE__) . "/empty.html");
assert($result === true);
?>
--EXPECTF--
%r(PHP ){0,1}%rWarning: DOMDocument::loadHTMLFile(): Document is empty %s
