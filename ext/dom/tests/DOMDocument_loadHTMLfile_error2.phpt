--TEST--
Test DOMDocument::loadHTMLFile when an empty string is passed
--DESCRIPTION--
Verifies that an error message is showed if an empty string is passed as argument
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--INI--
assert.bail=true
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument();
$result = $doc->loadHTMLFile("");
assert('$result === false');
?>
--EXPECTF--
%r(PHP ){0,1}%rWarning: DOMDocument::loadHTMLFile(): Empty string supplied as input %s
