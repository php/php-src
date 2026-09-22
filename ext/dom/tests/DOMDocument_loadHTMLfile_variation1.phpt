--TEST--
Test DOMDocument::loadHTMLFile when an empty document is loaded
--DESCRIPTION--
Verifies that an warning message is showed if an empty document is loaded
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (LIBXML_VERSION >= 21400) die("skip libxml >= 2.14 no longer has this non-standard warning");
?>
--FILE--
<?php
$doc = new DOMDocument();
$result = $doc->loadHTMLFile(__DIR__ . "/empty.html");
assert($result === true);
?>
--EXPECTF--
%r(PHP ){0,1}%rWarning: DOMDocument::loadHTMLFile(): Document is empty %s
