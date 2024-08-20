--TEST--
Dom\HTMLDocument::saveHtmlFile() empty path
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElement("root"));
$dom->saveHtmlFile("");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Dom\HTMLDocument::saveHtmlFile(): Argument #1 ($filename) must not be empty in %s:%d
Stack trace:
#0 %s(%d): Dom\HTMLDocument->saveHtmlFile('')
#1 {main}
  thrown in %s on line %d
