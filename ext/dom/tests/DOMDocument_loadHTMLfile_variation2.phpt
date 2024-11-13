--TEST--
Test DOMDocument::loadHTMLFile when a not-well formed document is loaded
--DESCRIPTION--
Verifies the behavior if a not-well formed document is loaded
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$result = $doc->loadHTMLFile(__DIR__ . "/not_well.html");
assert($result === true);
?>
--EXPECT--
