--TEST--
Test DOMDocument::loadHTMLFile when the file doesn't exist
--DESCRIPTION--
Verifies that an error message is showed if the file doesn't exist
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$result = $doc->loadHTMLFile(__DIR__ . "/ffff/test.html");
assert($result === false);
?>
--EXPECTF--
%r(PHP ){0,1}%rWarning: DOMDocument::loadHTMLFile(): I/O warning : failed to load external entity %s
